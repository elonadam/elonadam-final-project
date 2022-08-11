#ifndef INSTRUCT
#define INSTRUCT

#include "io.h"
#include "memoryList.h"

#define MAXOP 2 /* the maximum number of operands */

/* for the instructions that allow numbers as operands */
#define MAXOPNUM 127 /* the highest signed integer that can be held in 8 bits: 2^7 - 1 */
#define MINOPNUM -128  /* the lowest signed integer that can be held in 8 bits: -2^7 */

/*	count the number of operands the instruction ip can get and put the number in counter.
	ip is a pointer to the instruct.														*/
#define opcount(counter, ip)	{									\
									counter = 0;					\
									if (ip->opsc != AN) counter++;	\
									if (ip->optg != AN) counter++;	\
								}

#define ITYP -1 /*flag to identify insruction*/

#define ADSNUM 4 /* the number of addressing methods */

/* flags to sign what kind of addressing each operand in each instruction can be */
#define AN   0 /* no operand */
#define A0   1 /* can have a number as operand */
#define A1   2 /* can have a label as operand */
#define A2   4 /* can have a field of a struct as operand */
#define A3   8 /* can have a register as operand */
#define ACN -1 /* the addressing wasn't set */
#define AC0  0 /* the code of the A0 or there is no operand */
#define AC1  1 /* the code of the A1 */
#define AC2  2 /* the code of the A2 */
#define AC3  3 /* the code of the A3 */

/* define instructions and make an array of them, sorted alphabetically by their names */
typedef struct {
	string name; /* instruction's name */
	unsigned int opcode : OPCLEN; /* the instruction's code */
	unsigned int opsc : ADSNUM; /* addressing options for the source operand (each bit signal a specific addressing that is available) */
	unsigned int optg : ADSNUM; /* addressing options for the target operand (each bit signal a specific addressing that is available) */
} instruct;


/*	function that perform binary search to look for instruction and
	in the "insts".
	assume they are sorted alphabetically by names.
	return a pointer to the ones with matching name.
	if couldn't find any, return NULL				*/
instruct *findinst(string name);


#define INSTSIZE 16
extern instruct insts[];

#endif
