#include <stdio.h>
#include <stdlib.h>
#include "GenericHashTable.h"
#include "MyIntFunctions.h"
#include "TableErrorHandle.h"

#define FIRST_INT (-15)
#define LAST_INT 14
#define NUM_OF_PARAM 2

static void InsertAndPrint(TableP table);

int main(int argc, char* argv[]){
	int tableSize;
	TableP table;
	if(argc != NUM_OF_PARAM){
		printf("Usage: HashIntMain <tableSize>\n");
		exit(1);
	}
	tableSize = atoi(argv[1]);
	table = CreateTable(tableSize, IntFcn, IntPrint, IntCompare);
        if(NULL == table){
            ReportError(MEM_OUT);
            exit(1);
        }
	InsertAndPrint(table);
	return 0;
}

//Inserts the numbers from FIRST_INT to LAST_INT to the given table, and prints it
static void InsertAndPrint(TableP table){
	int i;
	int* key;
	ObjectP object;
	for(i=FIRST_INT; i<=LAST_INT; i++){
		key = (int*)malloc(sizeof(int));
		if(NULL == key){
                        FreeTable(table);
			ReportError(MEM_OUT);
			return;
		}
		(*key) = i;
		object = CreateObject(key);
		if(NULL == object){
                        free(key);
                        FreeTable(table);
			ReportError(MEM_OUT);
			return;
		}
		InsertObject(table, object);
	}
	PrintTable(table);
	FreeTable(table);
}
