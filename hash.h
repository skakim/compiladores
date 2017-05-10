// Amanda Torbes e Gabriel Moita

#ifndef HASH_HEADER
#define HASH_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 900

typedef struct hash_struct{
	int type;
	char *text;
	struct hash_struct *next;
} HASH_NODE;

HASH_NODE *hash_table[HASH_SIZE];
int elements_count;

HASH_NODE* hash_initialize();
HASH_NODE* hash_find(char *text, int address);
HASH_NODE* hash_insert(int type, char *text);
int hash_address(char *text);
void hash_print();

#endif
