#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "error.h"

string readline(FILE *fp, char *lc, int *lenline){
	string head; /* the new string */
	char *ptr; /* pointer for the characters of the string */
	char pc = BLK; /* the previous character (set to BLK to skip the white characters at the start of the line) */
	int len = 1; /* the length of the string (including EOS) */
	
	/* allocate space for the first character */
	head = (ptr = (string)malloc(sizeof(char)));
	
	/* check the allocation */
	if (head == NULL){
		return NULL;
	}
	
	/* read character and allocate memory for the next until EOF or NLN */
	while ((*ptr = fgetc(fp)) != EOF && *ptr != NLN){
		if (pc != BLK || (*ptr != BLK && *ptr != TAB)){ /* skip the current character if there are consecutive white characters */
			char *temp = (string)realloc((void *) head, ++len * sizeof(char)); /* allocate space for the next character */
			
			/* check the allocation */
			if (temp == NULL){
				free(head);
				return NULL;
			}
			
			pc = (*ptr = (*ptr == TAB) ? BLK : *ptr); /* if the current character is TAB, replace it with BLK */
			
			head = temp;
			ptr = head + len - 1; /* move ptr to the location of the new last character */
		}
	}
	
	*lc = *ptr; /* put the last character of the line (EOF or NLN) where lc points to */
	*ptr = EOS;
	
	/* check if the last character in the string is BLK and delete it (it is guaranteed to have at most one) */
	/* check if "0 < len - 1" to make sure that head[len - 2] is legal */
	if (0 < len - 1 && head[len - 2] == BLK){
		head[len - 2] = EOS;
	}
	
	if (lenline != NULL){
		*lenline = len;
	}
	
	return head;
}


int writeline(FILE *fp, string str){
	char lc = EOS; /* the last printed character (set to NLN to prevent printing NLN if the string is empty) */
	
	/* print the string */
	while (*str != EOS){
		if ((lc = fputc(*str, fp)) == EOF){ /* check if the printing was good */
			return ERR_PRINT;
		}
		
		str++;
	}
	
	/* check if the last character was NLN, and if not printing it */
	if (lc != NLN){
		if (fputc(NLN, fp) == EOF){ /* check if the printing was good */
			return ERR_PRINT;
		}
	}
	
	return SUCC;
}
