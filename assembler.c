/*
	this program compile ".as" files and writes to the standard output
	the errors if there are any.
	
	the errors that are reported, reffering to lines in the created ".am" file (unless written otherwise).
	
	if the compilation went successfully, ".ent", ".ext" and ".obj" files will be created.
	the ".ent" file is a list of labels declared in the current file that are aloowed to use
	in other files (it is posiable using the ".entry" guide).
	the ".ext" file is a list of labels from other files that are used in the current
	file (to import a label from other file, use the ".extern" guide).
	the ".obj" file is the original program and data created by the guides, after convertion to codes
	in base 32 characters representing them.
	
	authors: Elon Adam, Yoav Sayag. <3
*/


#include <stdio.h>
#include "assembler.h"
#include "table.h"
#include "error.h"


int main(int argc, char *argv[]){
	/* scan the argv array and compile each file  */
	while (0 < --argc){
		int status = SUCC; /* flag to remember if "pass1" found errors in the code */
		table tab; /* table for the labels */
		word *memhead = NULL; /* take the list of nodes representing the binary encoding of the operations (from "pass1") and pass it to "pass2" */
		
		printf("-------------       Compiling \"%s\"       -------------\n", argv[argc]);
		
		/* pre-compile the file and check for "fatal" errors */
		if ((status = pre(argv[argc])) == ERR_MEM || status == ERR_FOE || status == ERR_PRINT || status == ERR_FCE){
			return status;
		}
		
		/* perform the first pass on the pre-compiled file and check for fatal errors */
		if ((status = pass1(argv[argc], &tab, &memhead, status)) == ERR_MEM || status == ERR_FOE || status == ERR_PRINT || status == ERR_FCE){
			return status;
		}
		
		/* perform the second pass on the pre-compiled file and check for fatal errors */
		if ((status = pass2(argv[argc], status, tab, memhead)) == ERR_MEM || status == ERR_FOE || status == ERR_PRINT || status == ERR_FCE){
			return status;
		}
		
		if (status == SUCC){
			printf("------------- Compiled \"%s\" successfully -------------\n\n", argv[argc]);
		} else {
			printf("-------------   Failed to compile \"%s\"   -------------\n\n", argv[argc]);
		}
	}
	
	return SUCC;
}


