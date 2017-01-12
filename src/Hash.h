#include "anqi.hh"

class HASH{

	public:

		HASH();		
		void initial_hash_table();
		void insertHash(uint64_t, int, int, int);
		int searchHash(uint64_t, int, int, int);
		void deleteTabel();
		struct HashNode{
			int depth;
			int score;	// best value in this subtree
			int flag;	// 1: exact value  2: lower bound causing a beta cut  0 :null space
			uint64_t childB;	//?
		};

		uint64_t hashSize;
		HashNode *hashTable;
};



static const int FailSearch = -1000001;

HASH::HASH(){
	hashSize = 0xFFFFFFF;	//27 bit
	hashTable = (HashNode *) malloc(hashSize*sizeof(HashNode));
}


void HASH::initial_hash_table(){
	for (uint64_t i=0; i<hashSize; i++){
		hashTable[i].flag = 0;
	}
}


void HASH::insertHash(uint64_t key, int depth, int score, int flag){
	uint64_t addr = (key & hashSize);
	// 當該位址是空的 直接存入
	if(hashTable[addr].flag==0){
		hashTable[addr].depth = depth;
		hashTable[addr].score = score;
		hashTable[addr].flag = flag;
	}
	// 若有值 判斷depth
	else if(hashTable[addr].depth < depth){
		hashTable[addr].depth = depth;
		hashTable[addr].score = score;
		hashTable[addr].flag = flag;
	}
}

int HASH::searchHash(uint64_t key, int depth, int alpha, int beta){
	uint64_t addr = (key & hashSize);
	// Hash hit
	if (hashTable[addr].flag != 0){
		// 比目前的深度深
		if(hashTable[addr].depth >= depth){
			if (hashTable[addr].flag==1)
				return hashTable[addr].score;
			// 目前盤面的bound比Transition Table理得好
			if (depth%2==1 && hashTable[addr].score <= alpha)
				return alpha;
			if (depth%2==0 && hashTable[addr].score >= beta)
				return beta;
		}
		// Remember Best Move()
	}
	return FailSearch;
}

void HASH::deleteTabel(){
	free(hashTable);
}

