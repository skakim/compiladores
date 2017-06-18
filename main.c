// Amanda Torbes e Gabriel Moita

#include "lex.yy.h"
#include "astree.h"
#include "hash.h"
#include "y.tab.h"
#include "semantics.h"
#include "tac.h"

void initMe(void);
int isRunning(void);
int yyparse(void);

void readFile(int argc, char *argv[]){
	
	if(argc < 3){
		fprintf(stderr, "%s", "Missing file name! Command format: ./executable_program <input_name> <output_name> \n");
		exit(1); // arquivo nao informado
	}

	FILE* input = fopen(argv[1], "r");
	if(input == NULL){
		fprintf(stderr, "%s", "Can't open input file. \n");
		exit(2); // arquivo inexistente
	}

	yyin = input;

	yyparse();
	
}

int main(int argc, char *argv[]) {
	semanticErrors = 0;
	initMe();
	readFile(argc, argv);

	int numErrors = semanticFullCheck(ast);
	if(numErrors){
		fprintf(stderr,"%d erros semanticos.\n", numErrors);
		//astreePrint(ast,0);
		//hash_print();
		exit(4);
	}

	astreePrint(ast,0);
	//hash_print();

	FILE* output = fopen(argv[2], "w+");

	if(output == NULL){
		fprintf(stderr, "%s", "Can't open output file. \n");
		exit(2);
	}

	astreeProgram(ast,output); 
	tacPrintForward(tacReverse(tacGenerate(ast)));
	fclose(output);
	exit(0);
}
