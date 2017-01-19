/*****************************************************************************\
 * Theory of Computer Games: Fall 2012
 * Chinese Dark Chess Search Engine Template by You-cheng Syu
 *
 * This file may not be used out of the class unless asking
 * for permission first.
 *
 * Modify by Hung-Jui Chang, December 2013
 * Implement NegaScout & Eval by Yu-Shao Peng, Jan 2017
\*****************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <random>
#include "anqi.hh"
#include "Protocol.h"
#include "ClientSocket.h"
#include "Evaluation.h"
#include "HASH.h"

#ifdef _WINDOWS
#include<windows.h>
#else
#include<ctime>
#endif

const int PIECE = 15;	//1-14棋子 15未翻
const int LOCATION = 32;
const int PLAYER = 2;
const uint64_t RANDMAX = 0xFFFFFFFFFFFFFFFF; //64 bit maximum
int DEFAULTTIME = 15;
int remain_time;

HASH hashTable;
uint64_t state[PIECE][LOCATION];
uint64_t turn_who[PLAYER];

typedef  int SCORE;
static const SCORE INF=10000001;
static const SCORE WIN=10000000;

SCORE Max(SCORE, SCORE);

SCORE NegaScout(const BOARD&,int,int,int,int);
uint64_t getZobristKey(const BOARD&, int);
uint64_t random_generator();
void generate_random_state_turn();


#ifdef _WINDOWS
DWORD Tick;     // 開始時刻
int   TimeOut;  // 時限
#else
clock_t Tick;     // 開始時刻
clock_t TimeOut;  // 時限
#endif
MOV   BestMove; // 搜出來的最佳著法

bool TimesUp() {
#ifdef _WINDOWS
	return GetTickCount()-Tick>=TimeOut;
#else
	return clock() - Tick > TimeOut;
#endif
}


SCORE Eval(const BOARD &B, int dep) {
	Evaluation eva = Evaluation(B);
	int score = eva.material_value(dep);
	return score;
}

SCORE NegaScout(const BOARD &B, int alpha, int beta, int dep, int cut) {
	if(B.ChkLose())
		return -WIN;

	MOVLST lst;
	if(cut==0||TimesUp()||B.MoveGen(lst)==0){
		if (dep%2==0)
			return +Eval(B, dep);
		else
			return -Eval(B, dep);
	}
	MOV tmpBestMov = lst.mov[0];

	uint64_t key = getZobristKey(B, dep);
	int flag = hashTable.getFlag(key, cut);

	SCORE m = -INF;
	
	if (flag == 1){
		if (dep==0)
			BestMove = hashTable.getBestMov(key);
		return	hashTable.getExactVal(key);
	}
	else if(flag == 2){
		m = hashTable.getBound(key);
		tmpBestMov = hashTable.getBestMov(key);
		if (dep==0)
			BestMove = hashTable.getBestMov(key);
	}

	SCORE n = beta;	// the current upper bound
	
	for(int i=0; i<lst.num; i++) {
		BOARD N(B);
		N.Move(lst.mov[i]);
		const SCORE tmp = -1*NegaScout(N, -1*n, -1*Max(alpha, m), dep+1, cut-1);

		if(tmp==WIN && dep==0){
			BestMove=lst.mov[i];
			return WIN;
		}

		if(tmp>m) {
			if(n==beta||cut<3||tmp>=beta)
				m = tmp;
			else
				m = -1*NegaScout(N, -1*beta, -1*tmp, dep+1, cut-1);
			
			tmpBestMov = lst.mov[i];
			
			if(dep==0)
				BestMove=lst.mov[i];
		}
		if(m>=beta){
			hashTable.insertHash(getZobristKey(N, dep), 2, cut, m, tmpBestMov);
			return m;
		}
		n = Max(alpha, m) + 1;
	}
	hashTable.insertHash(getZobristKey(B, dep), 1, cut, m, tmpBestMov);
	return m;

}

SCORE Max(SCORE a, SCORE b){
	if(a>b)
		return a;
	else
		return b;
}

uint64_t getZobristKey(const BOARD &B, int depth){
	uint64_t key = 0x0;
	for (int i=0; i<LOCATION; i++){
		if (B.fin[i]<15){
			key = key^state[B.fin[i]][i];
		}
	}
	key = key^turn_who[depth%PLAYER];
	return key;
}

void generate_random_state_turn(){
	for (int i=0; i<PIECE; i++){
		for (int j=0; j<LOCATION; j++){
			state[i][j] = random_generator();
		}
	}
	for (int i=0; i<PLAYER; i++){
		turn_who[i] = random_generator();
	}
}

uint64_t random_generator(){
    static mt19937_64 local_rand(random_device{}());
    return uniform_int_distribution<uint64_t>(0, RANDMAX)(local_rand);
}


MOV Play(const BOARD &B) {

#ifdef _WINDOWS
	Tick=GetTickCount();
	TimeOut = (DEFAULTTIME-3)*1000;
#else
	Tick=clock();
	TimeOut = (DEFAULTTIME-3)*CLOCKS_PER_SEC;
#endif
	POS p; 
	int ITER_DEEP;
	SCORE scout_val;
	int c=0;
	// 由角落開局
	POS corner[4] = {0, 3, 28, 31};
	if(B.who==-1){p=corner[rand()%4];printf("%d\n",p);return MOV(p,p);}
	
	MOVLST lst;
	if (B.MoveGen(lst)!=0){
		// initial best move
		BestMove = lst.mov[0];
		
		// 當時間只剩下30秒 每步思考時間縮短為1秒
		if (remain_time < 30*1000)
			DEFAULTTIME=4;

		// 當時間只剩下100秒 每步思考時間縮短為3秒
		else if (remain_time < 100*1000)
			DEFAULTTIME=6;
		
		// 當合法步數小於10 深度減少
		if (lst.num < 10)
			ITER_DEEP=7;
		else
			ITER_DEEP=12;

		// Iterative Deepening
		for (int i=1; i<ITER_DEEP; i++){
			scout_val = NegaScout(B, -INF, INF, 0, i);
			if (scout_val==WIN)
				break;
		}
		// 若搜出來的結果會比現在好就用搜出來的走法
		if (scout_val>Eval(B,0)) return BestMove;		
	}

	// 否則翻子
	Evaluation eva = Evaluation(B);
	POS bestFin = eva.get_fin();
	if (bestFin==-1)
		return BestMove;
	return MOV(bestFin,bestFin);
}



FIN type2fin(int type) {
    switch(type) {
	case  1: return FIN_K;
	case  2: return FIN_G;
	case  3: return FIN_M;
	case  4: return FIN_R;
	case  5: return FIN_N;
	case  6: return FIN_C;
	case  7: return FIN_P;
	case  9: return FIN_k;
	case 10: return FIN_g;
	case 11: return FIN_m;
	case 12: return FIN_r;
	case 13: return FIN_n;
	case 14: return FIN_c;
	case 15: return FIN_p;
	default: return FIN_E;
    }
}
FIN chess2fin(char chess) {
    switch (chess) {
	case 'K': return FIN_K;
	case 'G': return FIN_G;
	case 'M': return FIN_M;
	case 'R': return FIN_R;
	case 'N': return FIN_N;
	case 'C': return FIN_C;
	case 'P': return FIN_P;
	case 'k': return FIN_k;
	case 'g': return FIN_g;
	case 'm': return FIN_m;
	case 'r': return FIN_r;
	case 'n': return FIN_n;
	case 'c': return FIN_c;
	case 'p': return FIN_p;
	default: return FIN_E;
    }
}

int main(int argc, char* argv[]) {

#ifdef _WINDOWS
	srand(Tick=GetTickCount());
#else
	srand(Tick=time(NULL));
#endif
	
	BOARD B;
	// initial random state
	generate_random_state_turn();
	hashTable.initial_hash_table();

	if (argc!=3) {
	    TimeOut=(B.LoadGame("board.txt")-3)*1000;
	    if(!B.ChkLose())Output(Play(B));
	    return 0;
	}
	Protocol *protocol;
	protocol = new Protocol();
	protocol->init_protocol(argv[1],atoi(argv[2]));
	int iPieceCount[14];
	char iCurrentPosition[32];
	int type;
	bool turn;
	PROTO_CLR color;

	char src[3], dst[3], mov[6];
	History moveRecord;
	protocol->init_board(iPieceCount, iCurrentPosition, moveRecord, remain_time);
	protocol->get_turn(turn,color);

	TimeOut = (DEFAULTTIME-3)*1000;

	B.Init(iCurrentPosition, iPieceCount, (color==2)?(-1):(int)color);

	MOV m;
	if(turn) // 我先
	{
	    m = Play(B);
	    sprintf(src, "%c%c",(m.st%4)+'a', m.st/4+'1');
	    sprintf(dst, "%c%c",(m.ed%4)+'a', m.ed/4+'1');
	    protocol->send(src, dst);
	    protocol->recv(mov, remain_time);
	    if( color == 2)
		color = protocol->get_color(mov);
	    B.who = color;
	    B.DoMove(m, chess2fin(mov[3]));
	    protocol->recv(mov, remain_time);
	    m.st = mov[0] - 'a' + (mov[1] - '1')*4;
	    m.ed = (mov[2]=='(')?m.st:(mov[3] - 'a' + (mov[4] - '1')*4);
	    B.DoMove(m, chess2fin(mov[3]));
	}
	else // 對方先
	{
	    protocol->recv(mov, remain_time);
	    if( color == 2)
	    {
		color = protocol->get_color(mov);
		B.who = color;
	    }
	    else {
		B.who = color;
		B.who^=1;
	    }
	    m.st = mov[0] - 'a' + (mov[1] - '1')*4;
	    m.ed = (mov[2]=='(')?m.st:(mov[3] - 'a' + (mov[4] - '1')*4);
	    B.DoMove(m, chess2fin(mov[3]));
	}
	B.Display();
	while(1)
	{
	    m = Play(B);

	    sprintf(src, "%c%c",(m.st%4)+'a', m.st/4+'1');
	    sprintf(dst, "%c%c",(m.ed%4)+'a', m.ed/4+'1');
	    protocol->send(src, dst);
	    protocol->recv(mov, remain_time);
	    m.st = mov[0] - 'a' + (mov[1] - '1')*4;
	    m.ed = (mov[2]=='(')?m.st:(mov[3] - 'a' + (mov[4] - '1')*4);
	    B.DoMove(m, chess2fin(mov[3]));
	    B.Display();

	    protocol->recv(mov, remain_time);
	    m.st = mov[0] - 'a' + (mov[1] - '1')*4;
	    m.ed = (mov[2]=='(')?m.st:(mov[3] - 'a' + (mov[4] - '1')*4);
	    B.DoMove(m, chess2fin(mov[3]));
	    B.Display();
	}

	return 0;
}
