#include <string.h>
#include <stdlib.h>
#include "table.h"
#include "io.h"
#include "error.h"
#include "supfuncs.h"


table createtab(){
	table tab = (table) malloc(HTS * sizeof(node *)); /* allocate space for the table */
	int i;
	
	/* check the allocation and set all the table's cells to NULL */
	if (tab != NULL){
		for (i = 0; i < HTS; i++) tab[i] = NULL;
	}
	
	return tab;
}


void freetab(table tab){
	int i;
	
	/* scan the table */
	for (i = 0; i < HTS; i++){
		node *ptr = tab[i]; /* pointer to the current list */
		
		/* scan the current list and free each node */
		while (ptr != NULL){
			node *temp = ptr->next; /* save the next node */
			
			free(ptr->key);
			free(ptr->content);
			free(ptr);
			
			ptr = temp; /* continue to the next node */
		}
	}
	
	free(tab); /* free the table itself */
}


unsigned int hash(string key){
	unsigned int hv = 0; /* the hash value of the key */
	
	/* calculate the hash value of the string */
	while (*key != EOS){
		hv += (*key++) + PMU * hv;
	}
	
	return hv % HTS;
}


int addtotabk(table tab, string key, string content, int kind){
	unsigned int hv = hash(key); /* the hash value of the key */ 
	node *newnode = (node *) malloc(sizeof(node)); /* the new node */
	node *temp; /* temporary node to swap nodes */
	string dupkey; /* new key to prevent changes to the key while it is in the table */
	string dupcont; /* new content to prevent changes to the content */
	
	/* check the allocation */
	isnull(newnode, ERR_MEM);
	
	/* duplicate the given name and content */
	if ((dupkey = dupl(key)) == NULL || (dupcont = duplstr(content)) == NULL){
		free(dupkey);
		free(dupcont);
		return ERR_MEM;
	}
	
	temp = tab[hv]; /* save the previous first node */
	tab[hv] = newnode; /* put the new node first */
	newnode->next = temp; /* put the previous first node after the new node */
	
	/* put the data of the new node in it */
	newnode->key = dupkey;
	newnode->content = dupcont;
	newnode->kind = kind;
	
	return SUCC;
}


string getcontent(table tab, string key){
	unsigned int hv = hash(key); /* the hash value of the  */
	node *ptr; /* pointer to scan the linked list */
	
	/* scan the list untill ptr is NULL or untill a node with the same key is found */
	for (	ptr = tab[hv]	;	ptr != NULL && strcmp(ptr->key, key)	;	ptr = ptr->next);
	
	/* return NULL if the node wasn't found. otherwise, return the content */
	return ptr == NULL ? NULL : ptr->content;
}


int getkind(table tab, string key){
	unsigned int hv = hash(key); /* the hash value of the  */
	node *ptr; /* pointer to scan the linked list */
	
	/* scan the list untill ptr is NULL or untill a node with the same key is found */
	for (	ptr = tab[hv]	;	ptr != NULL && strcmp(ptr->key, key)	;	ptr = ptr->next);
	
	/* return NOTFOUND if the node wasn't found. otherwise, return the kind */
	return ptr == NULL ? NOTFOUND : ptr->kind;
}
