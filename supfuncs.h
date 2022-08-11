#ifndef _sup
#define _sup

#include <stdlib.h>
#include "table.h"

/* adds 2 strings together, free the previous str1 and put the new string in str1 */
#define addfree(str1, str2) {										\
								char *temp = addstr(str1, str2);	\
								free(str1);							\
								str1 = temp;						\
							}

/* check if a pointer is NULL and return the given code */
#define isnull(ptr, code) {if (ptr == NULL) {return code;}}

/* swap between values of two given integers */
#define swap(x,y) {int temp = x; x = y; y = temp;}

/* get a string of "WORDSIZE" length and move all the characters two characters to the left (the two characters on the left are discarded) */
#define movel2(str) {int i; for (i = 2; i < WORDSIZE; i++) str[i - 2] = str[i];}

/* get a number and the bit that should be accessed, assume the bit is non-negative and that the bit is smaller then the number of bits in the type of num */
#define getbit(num, bit) (((num) & powr(BINARYBASE, (bit))) ? 1 : 0)

/* return the bit in position "bit" of the word (from right to left) */
#define gbw(str, bit) ((str)[(WORDSIZE - 1) - (bit)])


#define ONEARGS 1 /* only one argument in the current line */
#define TWOARGS 2 /* two argument in the current line */

#define REGSIZE 8 /* the number of registers */
extern string regs[]; /* declar the register array */

#define BINARYBASE 2 /* the binary base */
#define DECBASE 10 /* the decimal base */
#define BASE2TO32 5 /* the number of binary bits equivalent to one 32 base character */

extern char baseChars[]; /* declar the 32 base character array */


/*	convert string that represent binary number to 32 base.
	assume the string has 10 characters, and therefore the
	converted string will have two characters.
	return NULL if can't allocate memory.
	return pointer to the converted string if succeded.			*/
string convert32(string inputNum);


/*	return x to the power of y.
	assume x and y are non-negative integers.	*/
int powr(int x, int y);


/*	add two strings together and return the new string.
	return NULL if couldn't allocate memory for the
	new string.
	when finished using the string call the "free"
	function from stdlib.h in order to free the memory.		*/
char *addstr(char *, char *);


/*	get a string and check if it is a valid string declaration:
		check if the string is of the following format "macro NAME".
		the NAME is not a label, instruction, guide or register.
	there must be BLK only between every two "fields" and there
	can't be consectutive BLK.
	if it is a valid declaration, return a pointer to the start of the
	name in the string. otherwise, return NULL.			*/
string ismacrodec(table tab, string str);


/*	check if given string is label.
	check for forbidden names as reg, guide or instruction name.
	return null if use forbidden name.
	return ptr to string that is a copy of the label.		*/						
string islbl (string line);


/*	check if given string is a register.
	search legal register name syntax.
	returns NULL if illegal name.
	return pointer to string that equal to the name of the register. */
string isreg(string str);


/*	count chars until COM, BLK or EOS
	return the length of the field.		*/			
int sizeoffd(string str);


/*	count the number of fields in the string, any non white character
	can be the start of a field (including characters like: !@,#\/+)
	for example the following string has 6 fields:
	"hi . who % are u"
	the string must not be NULL. 					*/
int numoffds(string str);


/*	get int and returns its binary form (in two's compliment) as a string.
	the string has 10 characters and any bit that is after the tenth bit
	will be ignored.
	if couldn't allocate memory, return NULL.		*/
string itostr(int num); 


/*	duplicate the first field of the given string (pls free the memory with
	the "free" function from "stdlib.h").
	if couldn't allocate memory for the new string, return NULL.	*/
string dupl(string str);


/*	duplicate a 10-bit word (string).
	is the str is NULL, the function return an empty
	string (that should be freed later).
	free the memory with the "free" function from "stdlib.h".
	if couldn't allocate memory for the new string, return NULL.	*/
string duplw(string str);


/*	duplicate the  given string (pls free the memmory with
	the "free" function from "stdlin.h").
	if couldn't allocate memmory for the new string, return NULL.	*/
string duplstr(string str);


/*	skip field in given line.
	assume there is only one BLK between fields and nowhere else.
	return pointer to the start of the next field.
	if there is only one field, return pointer to the end of the string (EOS) */
string skipfd(string str);


/*	search for addressing method for given operand
	based on the operand's syntax.
	assume the number of fields in "struct" is one digit.
	return the legal addresing method (AC0, AC1, AC2, AC3)
	or ERR_OPD if illegel operand.				*/
int findadr(string str);


/*		split a line of command
		check the command and put a pointer to it in "command".
		put pointers to the arguments in "args" in the same order as in the line (adds '\0' to the end of each one).
		the function support strings (the first character of an operand most be QUM for it to be treated as string).
		put the number of arguments in "lenargs".
		in any case, use the "free" function from stdlib.h on "args" when finished using it.
	assume
		there is atleast one character in the line.
		the line doesn't start with blank
		the line doesn't contain tabs.
	return
		ERR_MEM			couldn't finish the task du to memmory shortage.
		ERR_OPT			unknown command name.
		ERR_COM_IL		illegal comma has been found.
		ERR_COM_MU		found multiple consecutive commas.
		ERR_COM_MI		there is a missing comma.
		ERR_OPD			a string wasn't properly closed.
		GTYP			finished successfully and found guide type.
		ITYP			finished successfully and found instruction type.
	warnning
		the function changes the content of the given string.           */
int splitline(string line, void **command, string **args, int *lenargs);


/*	free all given pointers.
	pointers has to point to memory allocated by calloc\malloc.						
	the last parameters has to be NULL.				*/
void freeall (void * ptr, ...);


/*	fclose all given FILE pointers.						
	the file has to be opend with fopen.
	even if couldn't close a file, the function 
	will try to close the next FILE pointers.
	return SUCC if succeded to close all
	and ERR_FCE if couldn't. 			*/
int fcloseall (FILE * ptr, ...);


#endif 
