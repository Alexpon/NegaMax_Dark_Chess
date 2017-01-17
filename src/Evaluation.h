#include <iostream>
#include <math.h>
#include "anqi.hh"
using namespace std;

class Evaluation{
public:
	Evaluation(const BOARD);
	BOARD B;
	int material_value(int);
	void dynamic_material_mode();
	int get_distancs(int, int);
	int get_fin();
	int material[16] = {1000,1200,200,50,10,200,5, 1000,1200,200,50,10,200,5, 0,0};
	// 使用技巧調整
	int fin_my_cannon[7] = {100, 100, 30, 20, 10, -100, 5};
	int fin_en_cannon[7] = {-100, -100, -10, -10, 0, 30, 0};
	int fin_my_neighbor[7] = {-50, 10, 5, 3, 1, -1000, 10};
	int fin_en_neighbor[7] = {10, -50, -10, -3, 0, 100, -5};

};


Evaluation::Evaluation(const BOARD b){
	B = b;
}

int Evaluation::material_value(int dep){
	int val[2]={0,0};
	int all_cnt[2]={0,0};
	int pins_num[2]={0,0};
	int king_num[2]={0,0};
	int my_color;
	int dis_val=0;
	int eva_val;

	if(dep%2==0)
		my_color = B.who;
	else
		my_color = B.who^1;
	// 計算目前盤面紅黑數量
	for(POS p=0;p<32;p++){
		const CLR c=GetColor(B.fin[p]);
		if(c!=-1){
			all_cnt[c]++;
			val[c] += material[B.fin[p]];
		}

		if(B.fin[p]==0 || B.fin[p]==7)
			king_num[c] = 1;
		else if(B.fin[p]==6 || B.fin[p]==13)
			pins_num[c] += 1;
	}
	// 計算尚未翻開棋子紅黑數量
	for(int i=0;i<14;i++){
		val[GetColor(FIN(i))] += B.cnt[i]*material[i];
		all_cnt[GetColor(FIN(i))] += B.cnt[i];
	}
	king_num[0] += B.cnt[0];
	king_num[1] += B.cnt[7];
	pins_num[0] += B.cnt[6];
	pins_num[1] += B.cnt[13];
/*
	if(all_cnt[my_color^1]<4 || (all_cnt[my_color]+all_cnt[my_color^1])<9){
		dis_val +=
	}
*/
	eva_val = val[my_color]-val[my_color^1] + king_num[my_color^1]*pow(9, (5-pins_num[my_color]));

	return eva_val;
}

void Evaluation::dynamic_material_mode(){
	
}

int Evaluation::get_distancs(int p1, int p2){
	int tmp = abs(p1-p2);
	return ( (tmp/4)+(tmp%4) );
}

int Evaluation::get_fin(){

	int best_soc = -100001;
	int tmp_soc;
	POS best_fin = -1;

	for (POS p=0; p<32; p++){
		if(B.fin[p]==FIN_X){
			tmp_soc = 0;
			// Cannon Place
			if( (p+2<32) && (((p+2)/4)==(p/4)) ){
				if ( GetColor(B.fin[p+2])==(B.who^1) )
					tmp_soc += fin_my_cannon[B.fin[p+2]%7];
				else if ( GetColor(B.fin[p+2])==(B.who) )
					tmp_soc += fin_en_cannon[B.fin[p+2]%7];
			}
			if( (p-2>=0) && (((p-2)/4)==(p/4)) ){
				if ( GetColor(B.fin[p-2])==(B.who^1))
					tmp_soc += fin_my_cannon[B.fin[p-2]%7];
				else if ( GetColor(B.fin[p-2])==(B.who))
					tmp_soc += fin_en_cannon[B.fin[p-2]%7];
			}
			if(p+8<32){
				if ( GetColor(B.fin[p+8])==(B.who^1) )
					tmp_soc += fin_my_cannon[B.fin[p+8]%7];
				else if ( GetColor(B.fin[p+8])==(B.who) )
					tmp_soc += fin_en_cannon[B.fin[p+8]%7];
			}
			if(p-8>=0){
				if ( GetColor(B.fin[p-8])==(B.who^1) )
					tmp_soc += fin_my_cannon[B.fin[p-8]%7];
				else if ( GetColor(B.fin[p-8])==(B.who) )
					tmp_soc += fin_en_cannon[B.fin[p-8]%7];
			}
			// Neighbor
			if( (p+1<32) && (((p+1)/4)==(p/4)) ){
				if ( GetColor(B.fin[p+1])==(B.who^1) )
					tmp_soc += fin_en_neighbor[B.fin[p+1]%7];
				if ( GetColor(B.fin[p+1])==B.who )
					tmp_soc += fin_my_neighbor[B.fin[p+1]%7];
			}
			if( (p-1>=0) && (((p-1)/4)==(p/4)) ){
				if ( GetColor(B.fin[p-1])==(B.who^1) )
					tmp_soc += fin_en_neighbor[B.fin[p-1]%7];
				if ( GetColor(B.fin[p-1])==B.who )
					tmp_soc += fin_my_neighbor[B.fin[p-1]%7];
			}
			if(p+4<32){
				if ( GetColor(B.fin[p+4])==(B.who^1) )
					tmp_soc += fin_en_neighbor[B.fin[p+4]%7];
				if ( GetColor(B.fin[p+4])==B.who )
					tmp_soc += fin_my_neighbor[B.fin[p+4]%7];
			}
			if(p-4>=0){
				if ( GetColor(B.fin[p-4])==(B.who^1) )
					tmp_soc += fin_en_neighbor[B.fin[p-4]%7];
				if ( GetColor(B.fin[p-4])==B.who )
					tmp_soc += fin_my_neighbor[B.fin[p-4]%7];
			}
			if(tmp_soc > best_soc){
				best_soc = tmp_soc;
				best_fin = p;
			}
		}
	}
	return best_fin;
	
}
