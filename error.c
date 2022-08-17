#include <stdio.h>
#include "error.h"
#include "io.h"
#include "memoryList.h"


void report(int error, int line) {
	
	/* check if it is a fatal error (general error)*/
	if (error == ERR_MEM || error == ERR_FOE || error == ERR_PRINT || error == ERR_FCE){
		printf("Fatal error: ");
	} else { /* error in the code provided by the user */
		printf("Error in line %3d: ", line);
	}
	
	switch(error){
		case ERR_MEM: 
			printf("Can't proceed due to memory problem.");
			break;
			
		case ERR_FOE:
			printf("Can't open file.");
			break;
			
		case ERR_PRINT:
			printf("Couldn't print to a file.");
			break;
		
		case ERR_FCE:
			printf("Couldn't close a file.");
			break;
			
		case ERR_COM_IL:
			printf("Illegal comma (comma somewhere before the first operand).");
			break;
			
		case ERR_COM_MI:
			printf("Missing comma between operands.");
			break;
			
		case ERR_COM_MU:
			printf("Multiple consecutive commas.");
			break;
			
		case ERR_LINE_EXR:
			printf("The line is too long (the limit is %d charcters), referring to the line in the \"%s\" file.", MAXLEN, EAS);
			break;
			
		case ERR_LINE_OLBL:
			printf("There is only label declaration in the line (no instruction or guide).");
			break;
			
		case ERR_OPD:
			printf("Illegal operand.");
			break;
			
		case ERR_OPT:
			printf("Unknown operation (no instruction or guide).");
			break;
			
		case ERR_ARGS_NUM:
			printf("Invalid number of operands.");
			break;
			
		case ERR_ARGS_LBL:
			printf("Unknown label in operand.");
			break;
			
		case ERR_ARGS_STC:
			printf("Label isn't struct.");
			break;
			
		case ERR_LBL:
			printf("The label already exists.");
			break;
			
		case ERR_ENT_LBL:
			printf("External label can't be entry.");
			break;
			
		case ERR_FULL_MEM:
			printf("Reached the end of the allowed memory (the limit is: %d).", MEM_END);
			break;
			
		case ERR_LMT_DAN:
			printf("Number too big or small for operand numbers.");
			break;
			
		case ERR_LMT_GUN:
			printf("Number too big or small for guide numbers.");
			break;
			
			
		case ERR_LBL_DEC:
			printf("Invalid label name in declaration.");
			break;
			
		case ERR_LBL_LEN:
			printf("Label name in label declaration is too long (the limit is: %d)", LBLLENMAX);

	}
	
	putchar(NLN); /* end the line */
			
	return;
}
