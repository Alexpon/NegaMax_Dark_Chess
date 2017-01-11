#include <iostream>
#include "anqi.hh"
using namespace std;

class Evaluation{
public:
	Evaluation(const BOARD);
	BOARD B;
	int material_value();
	void dynamic_material_mode();
	int fin_score();
};

Evaluation::Evaluation(const BOARD b){
	B = b;
}

int Evaluation::material_value(){
	int material[14] = {15,15,10,7,4,10,3, 15,15,10,7,4,10,3};
	int cnt[2]={0,0};

	// 計算目前盤面紅黑數量
	for(POS p=0;p<32;p++){
		const CLR c=GetColor(B.fin[p]);
		if(c!=-1)
			cnt[c] += material[B.fin[p]];
	}

	for(int i=0;i<14;i++)
		cnt[GetColor(FIN(i))]+=material[B.cnt[i]];						// 計算尚未翻開棋子紅黑數量
	return cnt[B.who]-cnt[B.who^1];
}

void Evaluation::dynamic_material_mode(){
	
}

int Evaluation::fin_score(){
	return 0;
}