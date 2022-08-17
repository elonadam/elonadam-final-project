/*	header for memory list
	this file as, definition of linked-list's node of two kinds:
		word - represent a memory word of the computer.
		lblword - lists for the extern and entry labels to print.
	
	also, there are macros and functions to easily manipulate the lists's strings
*/

#ifndef _MEM_LIST
#define _MEM_LIST

#include "supfuncs.h"


/* memory constants */
#define WORDSIZE	10	/* the length of memory word */
#define MEM_STRT	100	/* memory starting location */
#define MEM_END		255	/* memory end loacation */

/* the structure of the words */
#define ARELEN	2	/* the length of the ARE bits */
#define ARESTRT	0	/* the start position of the ARE bits*/
#define ADRLEN	8	/* the length of address */
#define ADRSTRT	2	/* teh position of the last significant bit of the address */
#define OPCLEN	4	/* the number of bits in the operation code part */
#define OPCSTRT	6	/* the position of the last significant bit of the op code */
#define ADSLEN	2	/* the length of the addressings in binary */
#define ADSTGST	2	/* the starting position of the target addressing */
#define ADSSCST	4	/* the starting position of the source addressing */
#define REGLEN	4	/* the length of register */
#define REGTGST	2	/* the starting position of the target register */
#define REGSCST	6	/* the starting position of the source register */

#define EXTADR "0000000001" /* the address given to external labels */

/* to set the ARE bits of a string representing a memory word */
#define __A 0 /* the memory word is absoulte */
#define __E 1 /* the memory word is external */
#define __R 2 /* the memory word is relocatable */
#define opcA(str) {int _i; for (_i = 0; _i < ARELEN; _i++) {gbw(str, _i + ARESTRT) = '0' + getbit(__A, _i);}} /* change the ARE bits of the memory word string to A */
#define opcE(str) {int _i; for (_i = 0; _i < ARELEN; _i++) {gbw(str, _i + ARESTRT) = '0' + getbit(__E, _i);}} /* change the ARE bits of the memory word string to E */
#define opcR(str) {int _i; for (_i = 0; _i < ARELEN; _i++) {gbw(str, _i + ARESTRT) = '0' + getbit(__R, _i);}} /* change the ARE bits of the memory word string to R */


/* define list of memory words */
typedef struct dataword {
	string data; /* the memory word */
	struct dataword *next; /* the next node */
} word;


/* define list of labels and their data */
typedef struct lblword {
	string name; /* the name of the label */
	string data; /* the data of the label (for example: line, address, etc...) */
	struct lblword *next; /* the next node */
} lblword;



/*	create a new "word" node with the given data,
	put the new node between "curr" and "cur->next".
	if "curr" is NULL, the function create a new node
	without setting it has a next node of a list.
	return the new node. if couldn't allocate memory
	for the new node, return NULL.						*/
word *addnext(word *curr, string data);


/*	create a new "lblword" node with the given name and data,
	put the new node between "curr" and "cur->next".
	if "curr" is NULL, the function create a new node
	without setting it has a next node of a list.
	return the new node. if couldn't allocate memory
	for the new node, return NULL.						*/
lblword *addnextlbl(lblword *curr, string name, string data);


/*	free the memory held by the given list.	*/
void freeMemList(word * head);


/*	free the memory held by the given list.	*/
void freeLblList(lblword * head);


/*	print to numbers and a "word" list
	to the given file, in the following format:
		num1 num2
		MEM_STRT	node_1.data
		MEM_STRT+1	node_2.data
		.
		.
		MEM_STRT+n	node_n.data
	
	everything is printed in base 32.
	assume the nodes' data is of length WORDSIZE
	and is a binary representation.
	
	return ERR_MEM if there is memory problem.
	return ERR_PRINT if couldn't print to the file.
	otherwise, return SUCC.						*/
int printMemList(FILE *fp, word *head, int num1, int num2);


/*	print "lblword" list to the given file.
	return ERR_MEM if there is memmory problem.
	return ERR_PRINT if couldn't print to the file.
	otherwise, return SUCC.						*/
int printLblList(FILE *fp, lblword *head);

#endif
