#include <iostream>
#include <math.h>
#include "anqi.hh"
using namespace std;

class Evaluation{
public:
	Evaluation(const BOARD);
	BOARD B;
	int material_value(int);
	int get_fin();
	int material[16] = {1000,1200,200,50,10,600,50, 1000,1200,200,50,10,600,50, 0,0};
	int fin_my_cannon[7] = {100, 100, 30, 20, 10, -100, 5};
	int fin_en_cannon[7] = {-100, -100, -10, -10, 0, 30, 0};
	int fin_my_neighbor[7] = {-50, 10, 5, 3, 1, -1000, 10};
	int fin_en_neighbor[7] = {10, -50, -10, -5, -5, 100, -10};

};


Evaluation::Evaluation(const BOARD b){
	B = b;
}

int Evaluation::material_value(int dep){
	int val[2]={0,0};
	int live_cheese[14]={0,0,0,0,0,0,0, 0,0,0,0,0,0,0};

	int my_color;
	int dis_val=0;
	int end_val=0;
	int enermy_num=0;
	int my_num=0;
	int my_best=-1, en_best=-1;
	int my_pos=-1, en_pos=-1;

	if(dep%2==0)
		my_color = B.who;
	else
		my_color = B.who^1;
	
	// 計算目前盤面翻開棋子數量及權重分數
	for(POS p=0;p<32;p++){
		const CLR c=GetColor(B.fin[p]);
		if(c!=-1){
			val[c] += material[B.fin[p]];
			live_cheese[B.fin[p]]+=1;
		}
	}

	// 計算尚未翻開棋子紅黑數量及權重分數
	for(int i=0;i<14;i++){
		val[GetColor(FIN(i))] += B.cnt[i]*material[i];
		live_cheese[i] += B.cnt[i];
	}

	// 計算我方及敵方剩餘棋子數量
	for (int i=0; i<7; i++){
		my_num += live_cheese[(my_color)*7+i];
		enermy_num += live_cheese[(my_color^1)*7+i];
	}

	// 殘局加分（當對方棋子少於四子時）
	if(enermy_num < 4){
		int idx=0;
		int e_lvl;
		int sub_lvl;
		int dis;

		dis_val += (4-enermy_num)*2000;
		for (POS p=0; p<32; p++){
			if(GetColor(B.fin[p])==(my_color^1)){
				e_lvl = B.fin[p]%7;
				for(POS p2=0; p2<32; p2++){
					if(GetColor(B.fin[p])==my_color){
						sub_lvl = -1*((B.fin[p])%7-e_lvl);
						dis = abs(p/4-p2/4) + abs(p-p2)%4;
						if (sub_lvl==6)
							dis_val += (-200)/dis;
						else if(sub_lvl==-6)
							dis_val += 200/dis;
						else{
							if(sub_lvl>0)
								sub_lvl*2;	// 吃掉對方比較重要
							dis_val += (1-(sub_lvl/6))*(sub_lvl*200/dis);
						}
					}
				}
				enermy_num--;
			}
			if(enermy_num==0)
				break;
		}
		
		// 取得雙方目前最大棋子
		for (int i=0; i<7; i++){
			if(live_cheese[my_color*7+i]!=0){
				my_best = my_color*7+i;
				break;
			}
		}
		for (int i=0; i<7; i++){
			if(live_cheese[(my_color^1)*7+i]!=0){
				en_best = (my_color^1)*7+i;
				break;
			}
		}

		// 處理雙方最大子 棋力相同的情形 關廁所
		if(my_best==en_best){
			for (POS p=0; p<32; p++){
				if(B.fin[p]==my_best)
					my_pos==p;
				if(B.fin[p]==en_best)
					en_pos==p;
			}
			if(my_pos!=-1 && en_pos!=-1){
				dis = (abs(my_pos/4-en_pos/4)+abs(my_pos-en_pos)%4);
				if (dis==1){
					if(dep%2==0)
						end_val=100000;
					else
						end_val=-100000;
				}
				else{
					if(enermy_num>my_num)
						end_val=3000/dis;
					else
						end_val=-3000/dis;
				}
			}
		}

		// 自己最大子無剋星加分
		else if(my_best>en_best)
			end_val=2000;
	}
	
	// 權重子力大小差距
	int pure_material = val[my_color]-val[my_color^1];
	// 兵卒權重根據數量以及對方將帥存活調整調整
	int pins_dynamic_val = live_cheese[(my_color^1)*7]*pow(9, (5-live_cheese[my_color*7+6]));
	// 將軍差距（為了彌補兵卒被降低價值的空缺）
	int king_live = (live_cheese[(my_color)*7]-live_cheese[(my_color^1)*7])*5000;
	
	return (pure_material + king_live + pins_dynamic_val + dis_val + end_val);

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
