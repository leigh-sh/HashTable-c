#include <stdio.h>
#include "MyIntFunctions.h"
#include "TableErrorHandle.h"

//Returns the hashcode of the given key
int IntFcn (const void *key, size_t tableSize){
	int k = *((int*)key);
	int hash = (k) % (int)(tableSize);
	if(hash < 0){
		hash += tableSize;
	}
	return hash;
}

//Prints the given key
void IntPrint (const void *key){
	int k = *((int*)key);
	printf("%d",(k));
}

//returns 1 if both keys are equal, 0 otherwise
int IntCompare (const void *key1, const void *key2){
	int k1 = *((int*)key1);
	int k2 = *((int*)key2);
	if(k1 == k2){
		return 1;
	}
	return 0;
}
