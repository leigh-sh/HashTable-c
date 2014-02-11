#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "GenericHashTable.h"
#include "TableErrorHandle.h"

#define MAX_OBJECTS 3
#define NOT_IN_LIST -1
#define DUP_FACTOR 2

typedef struct Node* NodeP;
typedef const struct Node* ConstNodeP;

struct Object{
	void* _key;
};

//A node in a linked list
struct Node{
	ObjectP _object;
	NodeP _next;
};

//Hash table
struct Table{
	NodeP* _arr;//The array of linkedLists
	size_t _origSize;
	size_t _curSize;
	HashFcn _hfun;
	PrintFcn _pfun;
	ComparisonFcn _fcomp;
	int _d;
};

ObjectP CreateObject(void *key);
TableP CreateTable(size_t tableSize, HashFcn hfun, PrintFcn pfun, ComparisonFcn fcomp);
Boolean InsertObject(TableP table, ObjectP object);
void FindObject(ConstTableP table, const void* key, int* arrCell, int* listNode);
void PrintTable(ConstTableP table);
void FreeObject(ObjectP object);
void FreeTable(TableP table);
ObjectP CreateObject(void *key);
TableP CreateTable(size_t tableSize, HashFcn hfun, PrintFcn pfun, ComparisonFcn fcomp);
static Boolean duplicateAndInsert(TableP table, NodeP node, int index);
static int listIndex(ConstTableP table, const void* key, int index);
static void freeList(ConstTableP table, int index);
static void printList(ConstTableP table, int index);
static NodeP* newListArray(size_t tableSize);

/*
 * Allocate memory for an object which points to the given key.
 * If run out of memory, free all the memory that was already allocated by the function, 
 * report error MEM_OUT to the standard error and return NULL.
 */
ObjectP CreateObject(void *key){
	ObjectP object;
	if(NULL == key){
		ReportError(GENERAL_ERROR);
		return NULL;
	}
	object = (ObjectP)malloc(sizeof(struct Object));
	if(NULL == object){
		ReportError(MEM_OUT);
		return NULL;
	}
	object->_key = key;
	return object;
}

/*
 * Allocate memory for a hash table with which uses the given functions.
 * tableSize is the number of cells in the hash table.
 * If run out of memory, free all the memory that was already allocated by the function, 
 * report error MEM_OUT to the standard error and return NULL.
 */
TableP CreateTable(size_t tableSize, HashFcn hfun, PrintFcn pfun, ComparisonFcn fcomp){
	TableP table;
	if(NULL == hfun || NULL == pfun || NULL == fcomp){
		ReportError(GENERAL_ERROR);
		return NULL;
	}
	table = (TableP)malloc(sizeof (struct Table));
	if(NULL == table){
		ReportError(GENERAL_ERROR);
		return NULL;
	}
	table->_arr = newListArray(tableSize);
	if(NULL == table->_arr){
		ReportError(MEM_OUT);
		free(table);
		return NULL;
	}
	table->_origSize = tableSize;
	table->_curSize = tableSize;
	table->_hfun = hfun;
	table->_pfun = pfun;
	table->_fcomp = fcomp;
	table->_d = 1;
	return table;
}

//Returns a new array of linkedLists which are initialized to NULL
static NodeP* newListArray(size_t tableSize){
	int i;
	NodeP* arr;
	arr = (NodeP*)malloc(sizeof(NodeP) * tableSize);
	if(NULL == arr){
		return NULL;
	}
	for(i=0; i<tableSize; i++){
		arr[i] = NULL;
	}
	return arr;
}

/*
 * Insert an object to the table.
 * If all the cells appropriate for this object are full, duplicate the table.
 * If run out of memory during the table duplication, report
 * MEM_OUT and do nothing (the table should stay at the same situation
 * as it was before the duplication).
 * If everything is OK, return TRUE. Otherwise (an error occured) return FALSE.
 */
Boolean InsertObject(TableP table, ObjectP object){
	int i;
	int j;
	int index;
	NodeP node;
	NodeP temp;
	if(NULL == table || NULL == object){
		ReportError(GENERAL_ERROR);
		return FALSE;
	}
	index = table->_hfun(object->_key, table->_origSize);
	index *= table->_d;
	node = (NodeP)malloc(sizeof(struct Node));
	if(NULL == node){
		ReportError(MEM_OUT);
		return FALSE;
	}
	node->_object = object;
	node->_next = NULL;
	for(j=index; j<index+table->_d; j++){
		temp = table->_arr[j];
		if(NULL == table->_arr[j]){
			table->_arr[j] = node;
			return TRUE;
		}
		i=1;
		while(NULL != temp->_next){
			temp = temp->_next;
			i++;
		}
		if(i < MAX_OBJECTS){
			temp->_next = node;
			return TRUE;
		}
	}
	return duplicateAndInsert(table, node, index);
}

//Multiplies the size of the table by 2. Puts the linkedLists in their
//approprite cells according to the exercise description
static Boolean duplicateAndInsert(TableP table, NodeP node, int index){
	int i;
	NodeP* newArr;
	if(NULL == table || NULL == node){
		return FALSE;
	}
	newArr = newListArray(table->_curSize * DUP_FACTOR);
	if(NULL == newArr){
		return FALSE;
	}
	for(i=0; i<table->_curSize * DUP_FACTOR; i++){
		if(i % DUP_FACTOR == 0){
			newArr[i] = table->_arr[i / DUP_FACTOR];
		}
		else{
			newArr[i] = NULL;
		}
	}
	newArr[index * DUP_FACTOR + 1] = node;
	free(table->_arr);
	table->_arr = newArr;
	table->_d = table->_d * DUP_FACTOR;
	table->_curSize *= DUP_FACTOR;
	return TRUE;
}

/*
 * Search the table and look for an object with the given key.
 * If such object is found fill its cell number into arrCell (where 0 is the
 * first cell), and its placement in the list into listNode (when 0 is the
 * first node in the list, i.e. the node that is pointed from the table
 * itself).
 * If the key was not found, fill both pointers with value of -1.
 */
void FindObject(ConstTableP table, const void* key, int* arrCell, int* listNode){
	int index;
	int inList;
	int i;
	if(NULL == table || NULL == key || NULL == arrCell || NULL == listNode){
		ReportError(GENERAL_ERROR);
		return;
	}
	index = table->_hfun(key, table->_origSize);
	index *= table->_d;
	for(i=index; i<index + table->_d; i++){
		inList = listIndex(table, key, i);
		if(inList != NOT_IN_LIST){
			assert(inList >= 0 && inList < MAX_OBJECTS);
			*arrCell = i;
			*listNode = inList;
			return;
		}
	}
	*arrCell = -1;
	*listNode = -1;
}

//Returns the index of the key in the list that starts with anchor, or NOT_IN_LIST
// if the key is not in the list
static int listIndex(ConstTableP table, const void* key, int index){
	int i=0;
	NodeP node = table->_arr[index];
	while(NULL != node){
		if(table->_fcomp(node->_object->_key, key)){
			return i;
		}
		node = node->_next;
		i++;
	}
	return NOT_IN_LIST;
}

/*
 * Print the table (use the format presented in PrintTableExample).
 */
void PrintTable(ConstTableP table){
	int i;
	if(NULL == table){
		ReportError(GENERAL_ERROR);
		return;
	}
	for(i=0; i<table->_curSize; i++){
		printf("[%d]\t",i);
		printList(table, i);
		printf("\n");
	}
}

//Prints the list table->_arr[index]
static void printList(ConstTableP table, int index){
	NodeP node = table->_arr[index];
	while(NULL != node){
		table->_pfun(node->_object->_key);
		printf("\t-->\t");
		node = node->_next;
	}
}

/*
 * Free all the memory allocated for an object, including the memory allocated
 * for the key.
 * Use this function when you free the memory of the table.
 */
void FreeObject(ObjectP object){
	if(NULL != object){
		free(object->_key);
		free(object);
	}
}

/*
 * Free all the memory allocated for the table.
 * It's the user responsibility to call this function before exiting the program.
 */
void FreeTable(TableP table){
	int i=0;
	if(NULL != table){
		for(i=0; i<table->_curSize; i++){
			freeList(table, i);
		}
		free(table->_arr);
		free(table);
	}
}

//Frees the list starting in anchor
static void freeList(ConstTableP table, int index){
	NodeP node = table->_arr[index];
	NodeP next;
	while(NULL != node){
		if(NULL == node->_next){
                        FreeObject(node->_object);
			free(node);
			node = NULL;
		}
		else{
			next = node->_next;
			FreeObject(node->_object);
			free(node);
			node = next;
		}
	}
}
