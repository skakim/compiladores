// Amanda Torbes e Gabriel Moita
// Compiladores - INF-UFRGS - 2017/1

#include "semantics.h"


// avaliacao recursiva da arvore
int semanticFullCheck(ASTREE *node){
	hashCheckUndeclared();	
	checkAstNodeDataType(node);
	checkSymbolsUse(node);
	return semanticErrors;
}


void setSymbolAndDataType(ASTREE *node, int type){

	if (type == AST_DEC_FUNC){
		if(node->son[0]->symbol->type != SYMBOL_IDENTIFIER){
		printSemanticError("funcao declarada mais de uma vez", node->son[0]->symbol->text, node->lineNumber);
		return;
		}
	}
	else if(node->symbol->type != SYMBOL_IDENTIFIER){
		printSemanticError("variavel declarada mais de uma vez", node->symbol->text, node->lineNumber);
		return;
	}
	switch(type){
		case AST_PARAM:	
			node->symbol->type = SYMBOL_VAR;
			setDataType(node, node->son[0]->type);
			break;
		case AST_DEC_VAR_GLOB: 				
			node->symbol->type = SYMBOL_VAR;
			setDataType(node, node->son[0]->type);
			break;
		case AST_DEC_VEC_GLOB: 
			node->symbol->type = SYMBOL_VEC;
			setDataType(node, node->son[0]->son[0]->type);
			break;
		case AST_DEC_FUNC:
			node->son[0]->symbol->type = SYMBOL_FUNC;
			setDataType(node->son[0], node->son[0]->son[0]->type);
			int n_par = countDecFuncNumParams(node->son[0]->son[1]);
			setNumParams(node,n_par);
			break;
	}
}

int countDecFuncNumParams(ASTREE *node){
	if(!node) // if NULL = end of list
		return 0;
	else
		return 1 + countDecFuncNumParams(node->son[1]);
}

void setNumParams(ASTREE *node, int npar){
	int address = hash_address(node->son[0]->symbol->text);
	HASH_NODE* hash = hash_find(node->son[0]->symbol->text, address);
	hash->num_params = npar;
}

void setDataType(ASTREE *node, int type){
	switch(type){
		case AST_BYTE: node->symbol->dataType = DATATYPE_BYTE; break;
		case AST_SHORT: node->symbol->dataType = DATATYPE_SHORT; break;
		case AST_LONG: node->symbol->dataType = DATATYPE_LONG; break;
		case AST_FLOAT: node->symbol->dataType = DATATYPE_FLOAT; break;
		case AST_DOUBLE: node->symbol->dataType = DATATYPE_DOUBLE; break;
		default: node->symbol->dataType = DATATYPE_UNDEFINED; break;
	}
	node->dataType = node->symbol->dataType;
}

void checkSymbolsUse(ASTREE *node){
	if(node == NULL){
		return;
	}
	
	switch(node->type){
		case AST_ASSIGN:
			if(node->symbol->type != SYMBOL_VAR){
				printSemanticError("expressao de atribuicao invalida",NULL, node->lineNumber);
			}
			break;
		case AST_VEC_ASSIGN:
			if(node->symbol->type != SYMBOL_VEC){
				printSemanticError("expressao de atribuicao de vetor invalida",NULL, node->lineNumber);
			}
			break;
		case AST_VECTOR_EXPR:
			if(node->symbol->type != SYMBOL_VEC){
				printSemanticError("expressao de acesso ao vetor invalida",NULL, node->lineNumber);
			}
			break;
		case AST_FUNC_CALL:
			if(node->symbol->type != SYMBOL_FUNC){
				printSemanticError("expressao de chamada de funcao invalida",NULL, node->lineNumber);		
			}			
			break;
		//Somente variaveis escalares sao aceitas no comando read e nao vetores ou pos. de vetores
		case AST_READ:
			if(node->symbol->type != SYMBOL_VAR){
				printSemanticError("comando 'read' invalido, apenas valores escalares sao aceitos",NULL, node->lineNumber);		
			}			
			break;		
	}

	int i;
	for(i=0; i<MAX_SONS; i++){
		checkSymbolsUse(node->son[i]);
	}
}

void verifyParams(ASTREE* node){
	int n_par = 0;
	if(node->son[0]){
		int val = verifyFuncCallParams(node->son[0]);
		if(!val) 
			printSemanticError("ha parametros booleanos na chamada da funcao",node->symbol->text, node->lineNumber);
		n_par = countFuncCallParams(node->son[0]);
	}		
	int address = hash_address(node->symbol->text);
	HASH_NODE* hash = hash_find(node->symbol->text, address);
	int correct_n_par = hash->num_params;
	if(n_par != correct_n_par)
		printSemanticError("numero invalido de parametros na chamada da funcao",node->symbol->text, node->lineNumber);
}

int verifyFuncCallParams(ASTREE* node){
	if(!node){
		return 1;
	}
	// undefined pq alem de bool, alguem pode tentar colocar uma string ou var q n exista
	if(node->son[0]->dataType == DATATYPE_BOOL || node->son[0]->dataType == DATATYPE_UNDEFINED){
		return 0; //indicativo de invalidez (algum parametro eh -1)
	}
	int acc_val = verifyFuncCallParams(node->son[1]);
	if(acc_val == 0){
		return 0; //se no resto da lista tem bool, retorna false
	} 
	
	return 1; //apenas retorna true se o tipo dessa nao eh booleano e nao tem nenhuma outra bool no resto da lista
}

int countFuncCallParams(ASTREE* node){
	if(!node)
		return 0;
	else
		return 1 + countFuncCallParams(node->son[1]);
}

int testID(HASH_NODE* id,ASTREE* node){
	if(id->type == SYMBOL_VEC || id->type == SYMBOL_FUNC){
		printSemanticError("uso invalido de vetor/funcao", id->text, node->lineNumber);
		node->dataType = DATATYPE_UNDEFINED;
		return 0;
	}
	return 1;
}


void checkAstNodeDataType(ASTREE *node){
	if(node == NULL){
		return;
	}

	int i;
	for(i=0; i<MAX_SONS; i++){
		checkAstNodeDataType(node->son[i]);
	}

	switch(node->type){
		case AST_SYMBOL:
			if(testID(node->symbol,node))
				node->dataType = node->symbol->dataType;
			break;		
		case AST_VECTOR_EXPR:
			if(node->son[0]->dataType != DATATYPE_LONG && node->son[0]->dataType != DATATYPE_SHORT
				 && node->son[0]->dataType != DATATYPE_CHAR && node->son[0]->dataType != DATATYPE_BYTE) {
				printSemanticError("indice do vetor deve ser do tipo inteiro", node->symbol->text, node->lineNumber); 
			}
			node->dataType = node->symbol->dataType;
			break;
		case AST_FUNC_CALL:
			verifyParams(node);
			node->dataType = node->symbol->dataType;
			break;
		case AST_LOGIC_L:
		case AST_LOGIC_G:
		case AST_LOGIC_LE:
		case AST_LOGIC_GE:
			if(node->son[0]->dataType == DATATYPE_BOOL || node->son[1]->dataType == DATATYPE_BOOL){
				printSemanticError("expressao booleana em operacao relacional", NULL, node->lineNumber);
			}
			if(node->son[0]->dataType == DATATYPE_UNDEFINED || node->son[1]->dataType == DATATYPE_UNDEFINED){
				printSemanticError("expressao UNDEFINED em operacao relacional", NULL, node->lineNumber);
				node->dataType = DATATYPE_UNDEFINED;
			}
			else
				node->dataType = DATATYPE_BOOL;
			break;
		case AST_LOGIC_EQ:
		case AST_LOGIC_NE:
			if((node->son[0]->dataType == DATATYPE_BOOL && node->son[1]->dataType != DATATYPE_BOOL) || 
			(node->son[1]->dataType == DATATYPE_BOOL && node->son[0]->dataType != DATATYPE_BOOL)){
				printSemanticError("conflito de tipos em operação de eq/ne", NULL, node->lineNumber);
			}
			if(node->son[0]->dataType == DATATYPE_UNDEFINED || node->son[1]->dataType == DATATYPE_UNDEFINED){
				printSemanticError("expressao UNDEFINED em operacao relacional", NULL, node->lineNumber);
				node->dataType = DATATYPE_UNDEFINED;
			}
			else
				node->dataType = DATATYPE_BOOL;
			break;
		case AST_LOGIC_AND:	
		case AST_LOGIC_OR:
			if(node->son[0]->dataType != DATATYPE_BOOL || node->son[1]->dataType != DATATYPE_BOOL){
				printSemanticError("expressao booleana esperada em operacao and/or", NULL, node->lineNumber); 
			}
			if(node->son[0]->dataType == DATATYPE_UNDEFINED || node->son[1]->dataType == DATATYPE_UNDEFINED){
				printSemanticError("expressao UNDEFINED em operacao relacional", NULL, node->lineNumber);
				node->dataType = DATATYPE_UNDEFINED;
			}
			else
				node->dataType = DATATYPE_BOOL;	
			break;
		case AST_LOGIC_NOT:
			if(node->son[0]->dataType != DATATYPE_BOOL){
				printSemanticError("expressao booleana esperada em operacao not", NULL, node->lineNumber); 
			}
			if(node->son[0]->dataType == DATATYPE_UNDEFINED){
				printSemanticError("expressao UNDEFINED em operacao NOT", NULL, node->lineNumber);
				node->dataType = DATATYPE_UNDEFINED;
			}
			else
				node->dataType = DATATYPE_BOOL;
			break;
		case AST_ADD:    
		case AST_SUB: 
		case AST_MUL: 
		case AST_DIV: 
			if(node->son[0]->dataType == DATATYPE_BOOL || node->son[1]->dataType == DATATYPE_BOOL){
				printSemanticError("expressao booleana nao esperada em expressao aritmetica", NULL, node->lineNumber);
			}
			if(node->son[0]->dataType == DATATYPE_UNDEFINED || node->son[1]->dataType == DATATYPE_UNDEFINED){
				printSemanticError("expressao UNDEFINED em operacao aritmetica", NULL, node->lineNumber);
				node->dataType = DATATYPE_UNDEFINED;
			}
			else
				node->dataType = aritmeticInference(node);
			break;
		case AST_ASSIGN: 
			if(!verifyAssignmentTypes(node->symbol->dataType, node->son[0]->dataType)){
				printSemanticError("conflito de tipos na atribuicao", NULL, node->lineNumber);
			}
			if(node->son[0]->dataType == DATATYPE_UNDEFINED){
				printSemanticError("expressao UNDEFINED em comando ASSIGN", NULL, node->lineNumber);
				node->dataType = DATATYPE_UNDEFINED;
			}
			else
				node->dataType = node->symbol->dataType;
			break;
		case AST_VEC_ASSIGN: 
			if(node->son[0]->dataType != DATATYPE_LONG && node->son[0]->dataType != DATATYPE_SHORT
				 && node->son[0]->dataType != DATATYPE_CHAR && node->son[0]->dataType != DATATYPE_BYTE) {
				printSemanticError("indice do vetor em atribuicao v#indice deve ser do tipo inteiro", node->symbol->text, node->lineNumber); 
			}
			if(!verifyAssignmentTypes(node->symbol->dataType, node->son[1]->dataType)){
				printSemanticError("conflito de tipos na atribuicao", NULL, node->lineNumber);
			}
			if(node->son[1]->dataType == DATATYPE_UNDEFINED){
				printSemanticError("expressao UNDEFINED em comando ASSIGN", NULL, node->lineNumber);
				node->dataType = DATATYPE_UNDEFINED;
			}
			else
				node->dataType = node->symbol->dataType;
			break;
		case AST_PARENTESIS_EXPR:
			node->dataType = node->son[0]->dataType;
			break;
		case AST_FOR:
			if(node->son[0]->dataType == DATATYPE_BOOL || node->son[1]->dataType == DATATYPE_BOOL){
				printSemanticError("expresao booleana em comando FOR", NULL, node->son[0]->lineNumber);	
			}
			if(node->son[0]->dataType == DATATYPE_UNDEFINED || node->son[1]->dataType == DATATYPE_UNDEFINED){
				printSemanticError("expressao UNDEFINED em comando FOR", NULL, node->son[0]->lineNumber);
			}	
			break;
		case AST_WHILE:
			if(node->son[0]->dataType != DATATYPE_BOOL){
				printSemanticError("expresao booleana esperada em comando WHILE", NULL, node->son[0]->lineNumber);	
			}
			if(node->son[0]->dataType == DATATYPE_UNDEFINED){
				printSemanticError("expressao UNDEFINED em comando WHILE", NULL, node->son[0]->lineNumber);
			}
			break;
		case AST_WHEN_THEN:
		case AST_WHEN_THEN_ELSE:
			if(node->son[0]->dataType != DATATYPE_BOOL){
				printSemanticError("expresao booleana esperada em comando WHEN", NULL, node->son[0]->lineNumber);
			}
			if(node->son[0]->dataType == DATATYPE_UNDEFINED){
				printSemanticError("expressao UNDEFINED em comando WHEN", NULL, node->son[0]->lineNumber);
			}
			break;		
		case AST_RETURN:	
			if(node->son[0]->dataType == DATATYPE_BOOL){
				printSemanticError("comando RETURN do tipo booleano nao esperado", NULL, node->lineNumber);
			}
			if(node->son[0]->dataType == DATATYPE_UNDEFINED){
				printSemanticError("expressao UNDEFINED em comando RETURN", NULL, node->lineNumber);
			}
			break;	
		case AST_READ:
			if(node->symbol->dataType == DATATYPE_BOOL || node->symbol->dataType == DATATYPE_UNDEFINED){
				printSemanticError("comando READ com tipo nao esperado", NULL, node->lineNumber);
			}
			break;
		// print é uma lista de elementos onde cada elemento pode ser um string ou expressao aritmetica
		case AST_PRINT_ELEM:
			if(node->symbol){
				if(node->symbol->dataType != DATATYPE_STRING){
					printSemanticError("comando PRINT com simbolo nao esperado, deve ser string", NULL, node->lineNumber);
				}
			}else if(node->son[0]){
				if(node->son[0]->dataType == DATATYPE_BOOL || node->son[0]->dataType == DATATYPE_UNDEFINED){
					printSemanticError("comando PRINT com expressao nao esperada, deve ser aritmetica", NULL, node->lineNumber);
				}
			}
	}

	//printf("type: %d datatype: %d \n", node->type, node->dataType);
}
int aritmeticInference(ASTREE *node){
	return typeInference(node->son[0]->dataType, node->son[1]->dataType);

}

int typeInference(int type1, int type2){
	if(type1 == DATATYPE_UNDEFINED || type2 == DATATYPE_UNDEFINED){
		return DATATYPE_UNDEFINED;
	}
	if(type1 == DATATYPE_BOOL || type2 == DATATYPE_BOOL){
		return DATATYPE_UNDEFINED;
	}

	if(type1 == DATATYPE_DOUBLE || type2 == DATATYPE_DOUBLE){
		return DATATYPE_DOUBLE;
	}
	else if(type1 == DATATYPE_FLOAT || type2 == DATATYPE_FLOAT){
		return DATATYPE_FLOAT;
	}
	else if(type1 == DATATYPE_LONG || type2 == DATATYPE_LONG){
		return DATATYPE_LONG;
	}
	else if(type1 == DATATYPE_SHORT || type2 == DATATYPE_SHORT){
		return DATATYPE_SHORT;
	}
	else if(type1 == DATATYPE_BYTE || type2 == DATATYPE_BYTE){
		return DATATYPE_BYTE;
	}else{ 
		return DATATYPE_UNDEFINED;
	}	
}

int verifyAssignmentTypes(int type1, int type2){
	if(type1 == DATATYPE_BOOL || type2 == DATATYPE_BOOL){
		return 0;
	}
	if(type1 == DATATYPE_UNDEFINED || type2 == DATATYPE_UNDEFINED){
		return 0;
	}
	return 1;
}

void hashCheckUndeclared(){
	HASH_NODE *node;
	int i;	 
	for(i=0; i<HASH_SIZE; i++){
		for(node=hash_table[i]; node; node=node->next){ 
			if(node->type == SYMBOL_IDENTIFIER){
				//erro variavel nao declarada
				printSemanticError("variavel nao declarada", node->text, node->lineNumber); 
			}
		}
	} 
}

void printSemanticError(char* errorDesc, char* param, int lineNumber){
	if(param != NULL){
		fprintf(stderr,"Erro semantico, linha %d: %s [%s].\n", lineNumber, errorDesc, param);	
	}else{
		fprintf(stderr,"Erro semantico, linha %d: %s.\n", lineNumber, errorDesc);	
	}
	semanticErrors++;	
}
