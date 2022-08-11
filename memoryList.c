#include <stdlib.h>
#include "memoryList.h"
#include "error.h"
#include "io.h"

word *addnext(word *curr, string data){
	word *newword; /* the new node that will be added to the list */
	word *tmp = NULL; /* to remember the previous "next" node of curr */
	string newcon; /* the duplicated content of the new node */
	
	newword = (word *)malloc(sizeof(word));
	isnull(newword, NULL); /* check the allocation */
	
	/* duplicate the given data - to prevent aliasing */
	if((newcon = duplw(data)) == NULL){
		free(newword);
		return NULL;
	}
	
	newword->data = newcon;
	
	/* if "curr" is not null, put the new node after it */
	if (curr != NULL){
		tmp = curr->next;
		curr->next = newword;
	}
	
	/* put the previous next after the new node */
	newword->next = tmp;
	
	return newword;
}


lblword *addnextlbl(lblword *curr, string name, string data){
	lblword *newword; /* the new node that will be added to the list */
	lblword *tmp = NULL; /* to remember the previous "next" node of curr */
	string newcon = NULL, newname = NULL; /* the duplicated content\name of the new node */
	
	newword = (lblword *)malloc(sizeof(lblword));
	isnull(newword, NULL); /* check the allocation */
	
	/* duplicate the given data - to prevent aliasing */
	newname = dupl(name == NULL ? ESTR : name); /* if the name is NULL, create an empty string */
	if(newname == NULL || (newcon = duplw(data)) == NULL){
		freeall(newword, newname, newcon);
		return NULL;
	}
	
	newword->name = newname;
	newword->data = newcon;
	
	/* if "curr" is not null, put the new node after it */
	if (curr != NULL){
		tmp = curr->next;
		curr->next = newword;
	}
	
	/* put the previous next after the new node */
	newword->next = tmp;
	
	return newword;
}


void freeMemList(word * head){
	word * tmp; /* node to remember the next node */
	
	/* scan the list and free each node and its field */
	while(head != NULL) {
		tmp = head -> next;
		
		free(head -> data);
		free(head);
		
		head = tmp; /* continue to the next node */
	}
	
	return;
}


void freeLblList(lblword * head){
	lblword * tmp; /* node to remember the next node */
	
	/* scan the list and free each node and its fields */
	while(head != NULL) {
		tmp = head -> next;
		
		freeall(head -> name, head->data, NULL);
		free(head);
		
		head = tmp; /* continue to the next node */
	}
	
	return;
}


int printMemList(FILE *fp, word *head, int num1, int num2){
	int memc = MEM_STRT; /* memmory counter */
	string tmp; /* to check for ERR_MEM */
	string str1, str2; /* the binary representation of num1 and num2 */
	
	/* convert num1 to binary representation */
	if((tmp = itostr(num1)) == NULL){
		return ERR_MEM;
	}
	
	/* convert num1 to base 32 representation */
	if((str1 = convert32(tmp)) == NULL){
		free(tmp);
		return ERR_MEM;
	}
	
	free(tmp);
	
	/* convert num2 to binary representation */
	if((tmp = itostr(num2)) == NULL){
		free(str1);
		return ERR_MEM;
	}
	
	if((str2 = convert32(tmp)) == NULL){ /*check if succeded to convert to base 32*/
		freeall(str1, tmp, NULL);
		return ERR_MEM;
	}
	free(tmp);
	
	/* write to the file coded numbers */
	if (write2str(fp, str1, str2) < 0) {
		freeall(str1, str2, NULL);
		return ERR_PRINT;
	}
	
	
	/* scan the list */
	while (head != NULL) {
		string adrs32, data32; /* the data needed to be printed */
		
		/* convert the number to binary */
		if ((tmp = itostr(memc++)) == NULL){
			return ERR_MEM;
		}
		
		/* convert to base 32 */
		if ((adrs32 = convert32(tmp)) == NULL){
			free(tmp);
			return ERR_MEM;
		}
		
		free(tmp);
		
		/* convert the data of the current node to base 32 */
		if ((data32 = convert32(head->data)) == NULL){
			free(adrs32);
			return ERR_MEM;
		}
		
		/* print to the given file */
		writeline(fp, ESTR);
		if (write2str(fp, adrs32, data32) < 0){
			freeall(adrs32, data32, NULL);
			return ERR_PRINT;
		}
		
		freeall(adrs32, data32, NULL);
		
		head = head->next; /* continue to the next node */
	}
	
	return SUCC;
}


int printLblList(FILE *fp, lblword * head){
	int firstline = 1; /* flag to note that we are on the first node */
	
	/* scan the list */
	while (head != NULL) {
		string code32 = convert32(head->data); /* the data needed to be printed */
		
		isnull(code32, ERR_MEM); /* check convert32 */
		
		/* if it's not the first line, seperate the new line from the previous one */
		if (firstline){
			firstline = !firstline; /* turn off the flag */
		} else {
			writeline(fp, ESTR); /* seperate the lines */
		}
		
		/* print the numbers to the given file */
		if(write2str(fp, head->name, code32) < 0){
			free(code32);
			return ERR_PRINT;
		}
		free(code32);
		
		head = head->next; /* continue to the next node */
	}
	
	return SUCC;
}
