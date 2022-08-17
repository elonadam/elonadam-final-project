/*
	header for the prototypes of the pre-assembler, pass1 and pass2 functions.
	which are responsiable for compiling an EAS file to EAM,EOB,ENT and EXT files.
*/


#include "memoryList.h"
#include "io.h"
#include "table.h"


/*	take "path" and read the file named "path" with the extension EAS.
	write to the file named "path" with the extension EAM the lines of the
	EAS file.
	any unnecessary spaces and tabs are removed, comment lines are removed,
	and all the macros are expended.
	
	the function does checks for:
		ERR_MEM
		ERR_FOE
		ERR_PRINT
		ERR_FCE
		ERR_LINE_EXR
	
	and return the code of the error in case of error, return
	SUCC otherwise.																*/
int pre(char *path);


/*	take "path" and read the file named "path" with the extension EAM.
	create table of the labels found in the file and put it in "tab".
	create a list of the operations' encoding and put a pointer to the head
	in "memhead".
	
	the function does checks for:
		ERR_MEM
		ERR_FOE
		ERR_FCE
		ERR_LINE_OLBL
		ERR_OPD
		ERR_ARGS_NUM
		ERR_LBL
		ERR_FULL_MEM
		ERR_LMT_GUN
		and the checks that "splitline" from "supfuncs.h" perform.
	
	and return the code of the error in case of error, return
	SUCC otherwise (the status parameter is there to change the default from SUCC).		*/
int pass1(string path, table *tab, word **memhead, int status);



/*	take "path" and read the file named "path" with the extension EAM.
	use the labels in "tab" to create the operands' encoding.
	had the operands' encoding to the "memhead" list in the right position
	based on the "path" file, and print the encoded list to a file named
	"path" with the extension of EOB.
	print to a file named "path" with the extension ENT a list of labels
	that are declared as "entry" in the EAM file.
	print to a file named "path" with the extension EXT a list of labels
	that are declared as "extern" in the EAM file.
	
	the function does checks for:
		ERR_MEM
		ERR_FOE
		ERR_PRINT
		ERR_FCE
		ERR_LINE_OLBL
		ERR_OPD
		ERR_ARGS_NUM
		ERR_ARGS_LBL
		ERR_ARGS_STC
		ERR_ENT_LBL
		ERR_LMT_DAN
		and the checks that "splitline" from "supfuncs.h" perform.
	
	if status is not SUCC or found an error the ENT, EXT and EOB files will be empty
	
	and return the code of the error in case of error, return
	SUCC otherwise.																*/
int pass2(string path, int status, table tab, word *memhead);
