// Amanda Torbes e Gabriel Moita

#include "hash.h"

HASH_NODE* hash_initialize(){
	int i;
    *hash_table = calloc(HASH_SIZE, sizeof(HASH_NODE));
	for(i=0; i<HASH_SIZE; i++){
		hash_table[i] = 0;
	}
	elements_count = 0;
}

HASH_NODE* hash_find(char *text, int address){
	HASH_NODE *node;
	for(node=hash_table[address]; node; node=node->next){
		if(strcmp(text, node->text) == 0){
			return node;
		}
	}	
	
	return 0;
}

HASH_NODE* hash_insert(int type, char *text){	
	HASH_NODE *new_node = calloc(1, sizeof(HASH_NODE));
	HASH_NODE *exist_node;
	HASH_NODE *first_node;
	int address;
	address = hash_address(text); 	
	if(exist_node = hash_find(text, address)){
		return exist_node;
	}
	new_node->type = type;
	new_node->text = calloc(strlen(text)+1, sizeof(char));	
	strcpy(new_node->text, text);
	new_node->next = hash_table[address];
	hash_table[address] = new_node;
	elements_count++;

	return hash_table[address];
}

int hash_address(char *text){
	int address = 1;
	int i;
	for(i=0; i<strlen(text); i++){
		address = (address * text[i]) % HASH_SIZE + 1;
	}
	
	return address - 1;
}

void hash_print(){
	HASH_NODE *node;
	int i;	
	for(i=0; i<HASH_SIZE; i++){
		for(node=hash_table[i]; node; node=node->next){
			printf("[Table] Type: %d - Text: %s\n", node->type, node->text);
		}
	} 
}

void hash_ckeck_undeclared(){
	HASH_NODE *node;
	int i;	
	for(i=0; i<HASH_SIZE; i++){
		for(node=hash_table[i]; node; node=node->next){
			if(node->type == SYMBOL_IDENTIFIER){
				//erro variavel ja declarada
				printf("Erro: variavel %s já declarada.\n", node->text);
			}
		}
	} 
}
