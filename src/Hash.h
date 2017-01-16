#include "anqi.hh"

class HASH{

	public:

		HASH();		
		void initial_hash_table();
		void insertHash(uint64_t, int, int, int);
		int searchHash(uint64_t, int, int, int, int);
		int getFlag(uint64_t, int);
		int getExactVal(uint64_t);
		int getBound(uint64_t);

		struct HashNode{
			int depth;
			int score;	// best value in this subtree
			int flag;	// 1: exact value  2: bound  0 :null space
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


void HASH::insertHash(uint64_t key, int flag, int cut, int score){
	uint64_t addr = (key & hashSize);
	// 當該位址是空的 直接存入
	if(hashTable[addr].flag==0){
		hashTable[addr].depth = cut;
		hashTable[addr].score = score;
		hashTable[addr].flag = flag;
	}
	// 若有值 判斷depth
	else if(hashTable[addr].depth < cut){
		hashTable[addr].depth = cut;
		hashTable[addr].score = score;
		hashTable[addr].flag = flag;
	}
}

int HASH::getFlag(uint64_t key, int cut){
	uint64_t addr = (key & hashSize);
	if (hashTable[addr].flag==0)
		return 0;
	if (hashTable[addr].depth >= cut)
		return hashTable[addr].flag;
	else
		return 0;
}

int HASH::getExactVal(uint64_t key){
	uint64_t addr = (key & hashSize);
	return hashTable[addr].score;
}

int HASH::getBound(uint64_t key){
	uint64_t addr = (key & hashSize);
	return hashTable[addr].score;
}

