// Amanda Torbes e Gabriel Moita

#ifndef HASH_HEADER
#define HASH_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_defines.h"

#define HASH_SIZE 900

typedef struct hash_struct{
	int type;
	int dataType;
	int num_params;
	char *text;
	int lineNumber;
	struct hash_struct *next;
} HASH_NODE;

HASH_NODE *hash_table[HASH_SIZE];
int elements_count;

HASH_NODE* hash_initialize();
HASH_NODE* hash_find(char *text, int address);
HASH_NODE* hash_insert(int type, char *text, int dataType, int lineNumber);
int hash_address(char *text);
void hash_print();


#endif
