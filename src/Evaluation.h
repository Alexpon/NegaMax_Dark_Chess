#include <iostream>
#include "anqi.hh"
using namespace std;

class Evaluation{
public:
	Evaluation(const BOARD);
	BOARD B;
	int material_value(int);
	void dynamic_material_mode();
	int get_fin();
	int material[16] = {15,15,10,7,4,10,3, 15,15,10,7,4,10,3, 0,0};
	//int fin_cannon[7] = {50, 40, 20, 15, 10, -100, 5};
	int fin_my_neighbor[7] = {0, 10, 5, 3, 1, -100, 10};
	int fin_en_neighbor[7] = {10, -50, -10, -3, 0, 100, -5};

};


Evaluation::Evaluation(const BOARD b){
	B = b;
}

int Evaluation::material_value(int dep){
	int cnt[2]={0,0};

	// 計算目前盤面紅黑數量
	for(POS p=0;p<32;p++){
		const CLR c=GetColor(B.fin[p]);
		if(c!=-1)
			cnt[c] += material[B.fin[p]];
	}

	for(int i=0;i<14;i++)
		cnt[GetColor(FIN(i))]+=material[B.cnt[i]];						// 計算尚未翻開棋子紅黑數量
	if(dep%2==0)
		return cnt[B.who]-cnt[B.who^1];
	else
		return cnt[B.who^1]-cnt[B.who];
}

void Evaluation::dynamic_material_mode(){
	
}

int Evaluation::get_fin(){
	int fin_cannon[7] = {50, 40, 20, 15, 10, -100, 5};

	int best_soc = -100001;
	int tmp_soc;
	POS best_fin = -1;

	for (POS p=0; p<32; p++){
		if(B.fin[p]==FIN_X){
			tmp_soc = 0;
			// Cannon Place
			if( (p+2<32) && (((p+2)/4)==(p/4)) ){
				if (GetColor(B.fin[p+2])==B.who^1){
					tmp_soc += fin_cannon[B.fin[p+2]%7];
				}
			}
			if( (p-2>=0) && (((p-2)/4)==(p/4)) ){
				if (GetColor(B.fin[p-2])==B.who^1)
					tmp_soc += fin_cannon[B.fin[p-2]%7];
			}
			if(p+8<32){
				if (GetColor(B.fin[p+8])==B.who^1)
					tmp_soc += fin_cannon[B.fin[p+8]%7];
			}
			if(p-8>=0){
				if (GetColor(B.fin[p-8])==B.who^1)
					tmp_soc += fin_cannon[B.fin[p-8]%7];
			}
			// Neighbor
/*
			if( (p+1<32) && (((p+1)/4)==(p/4)) ){
				if (GetColor(B.fin[p])==who^1)
					tmp_soc += fin_en_neighbor[B.fin[p+1]%7];
				if (GetColor(B.fin[p])==who)
					tmp_soc += fin_my_neighbor[B.fin[p+1]%7];
			}
			if( (p-1>=0) && (((p-1)/4)==(p/4)) ){
				if (GetColor(B.fin[p])==who^1)
					tmp_soc += fin_en_neighbor[B.fin[p-1]%7];
				if (GetColor(B.fin[p])==who)
					tmp_soc += fin_my_neighbor[B.fin[p-1]%7];
			}
			if(p+4<32){
				if (GetColor(B.fin[p])==who^1)
					tmp_soc += fin_en_neighbor[B.fin[p+4]%7];
				if (GetColor(B.fin[p])==who)
					tmp_soc += fin_my_neighbor[B.fin[p+4]%7];
			}
			if(p-4>=0){
				if (GetColor(B.fin[p])==who^1)
					tmp_soc += fin_en_neighbor[B.fin[p-4]%7];
				if (GetColor(B.fin[p])==who)
					tmp_soc += fin_my_neighbor[B.fin[p-4]%7];
			}*/
			if(tmp_soc > best_soc){
				best_soc = tmp_soc;
				best_fin = p;
			}
		}
	}
	return best_fin;
	
	// 隨便翻一個地方
	/*	
	int c = 0;
	POS p;
	for(p=0;p<32;p++)if(B.fin[p]==FIN_X)c++;
	if(c==0)return -1;
	c=rand()%c;
	for(p=0;p<32;p++)if(B.fin[p]==FIN_X&&--c<0)break;
	return p;
	*/
}
