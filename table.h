/* 
	header for the prototypes and macros of the table functions.
	the current structure is an "hash table".
*/


#ifndef TABLE_H
#define TABLE_H
#include "io.h"


#define HTS 41 /* the size of the hash table array */
#define PMU 31 /* prime scalar - set the distribution in the table */
#define DEFK 0 /* the default kind of information */
#define NOTFOUND -1 /* "getking" return this value if couldn't find label */

/* eqivalent to addtotabk but set the kind to DEFK */
#define addtotab(tab, key, content) addtotabk(tab, key, content, DEFK)


/* the nodes of the lists in the table */
typedef struct node {
	string key; /* the name of the node */
	string content; /* the content */
	int kind; /* optional - a way to categorize the content, for example:
				1 - the content is a name.
				2 - the content is an age.
				3 - the content is a date.			*/
	struct node *next; /* the next node in the list */
} **table; /* hash table */

typedef struct node node; /* for convenience */


/*	create a table.
	when not needed anymore, please
	free the table using the "freetab" functions	*/
table createtab();


/*	free the memory taken by the table (delete and free all of its content)	*/
void freetab(table tab);


/*	add new information to the table.
	tab - the table to add the info to.
	key - the name of the content.
	kind - the kind of information.
	content - the content to save.
	
	return ERR_MEM if there was a problem with memory allocation.
	otherwise, return SUCC.						*/
int addtotabk(table tab, string key, string content, int kind);


/*	search in the table for the content with a name matching
	key, and return the content.
	if couldn't find anything, return NULL.				*/
string getcontent(table tab, string key);

/*	search in the table for the kind with a name matching
	key, and return the kind.
	if couldn't find anything, return NULL.				*/
int getkind(table tab, string key);

#endif
