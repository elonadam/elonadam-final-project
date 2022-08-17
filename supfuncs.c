#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include "io.h"
#include "preasm.h"
#include "supfuncs.h"
#include "table.h"
#include "guides.h"
#include "error.h"
#include "instructs.h"
#include "memoryList.h"

string addstr(string str1, string str2){
	string nstr; /* the new string */
	string fstr1 = (str1 == NULL ? ESTR : str1); /* if the string is NULL, replace it with ESTR */
	
	/* allocate space for the new string */
	nstr = (string)malloc(  (strlen(fstr1) + strlen(str2) + 1) * sizeof(char)  );
	
	/* check the allocation */
	if (nstr != NULL){
		strcpy(nstr, fstr1);
		strcat(nstr, str2);
	}
	
	return nstr;
}


string ismacrodec(table tab, string str){
	string name; /* the name of the macro */
	string macro = MAC; /* pointer to the macro keyword */
	int i = 0;
	
	/* count how many characters in the first field of str are identical to MAC */
	while (str[i] == macro[i] && str[i] != EOS && macro[i] != EOS) i++;
	
	
	/* check if the first field is equal to MAC */
	if (!(i == strlen(macro) && str[i] == BLK)){
		return NULL; /* the line is not a macro declaration */
	}
	
	/*	under the assumption that there is BLK only between every two fields and
		there can't be consecutive BLK, there must be a second field.		*/
	
	/* put the start of the second field in "name" */
	name = str + (++i);
	
	/* find the end of the second field */
	while (str[i] != EOS && str[i] != BLK) i++;
	
	if (str[i] == BLK){ /* more than two fields */
		return NULL;
	}
	
	
	/* the string has two fields and the first is "macro" */
	
	
	/* check if there already is a macro with this name */
	if (getcontent(tab, name) != NULL){
		return NULL;
	}
	
	
	/* check if the name is an instruction */
	if (findinst(name) != NULL){
		return NULL;
	}
	
	
	/* check if the name is a register */
	if (isreg(name) != NULL){
		return NULL;
	}
	
	
	/* check if the name is a name of a guide (with or without the prefix) */
	
	/* check if it starts with the prefix */
	if (*name == DOT){
		name++;
	}
	
	/*	if the name doesn't start with DOT, check if name+0 (which is equal to name) is one of the guides's names
		if the name does start with DOT, check if name+1 (from the end of the prefix) is one of the guides's names	*/
	if (findguide(name) != NULL){
		return NULL;
	}
	
	/* it is a valid declaration */
	return name;
}


string islbl(string line){
	string lbl; /* the duplicated label */
	char lc; /* the character after the first field in the line (EOS or BLK) */
	int i;
	
	/* check if the first character is alphabet */
	if (!isalpha(*line)) return NULL;
	
	/* count the number of characters in the label, and check that all of them are alpha-numeric */
	for (i = 0; line[i] != COL && line[i] != BLK && line[i] != EOS; i++){
		if (!isalnum(line[i])){ /* check the character */
			return NULL;
		}
	}
	
	/* check that the last character in the field is COL */
	if (line[i] != COL || (line[i+1] != BLK && line[i + 1] != EOS)){
		return NULL;
	}
	
	 /* saev the first character after the first field and replace it with EOS*/
	lc = line[i];
	line[i] = EOS;
	
	/* chek if the label is a name of guide */
	if (findguide(line) != NULL){
		line[i] = lc; /* put the character back */
		return NULL;
	}
	
	/* chek if the label is a name of instruction */
	if (findinst(line) != NULL){
		line[i] = lc; /* put the character back */
		return NULL;
	}
	
	/* chek if the label is a name of register */
	if (isreg(line) != NULL){
		line[i] = lc; /* put the character back */
		return NULL;
	}
	
	/* duplicate the label */
	lbl = dupl(line);
	
	line[i] = lc; /* put the character back */
	
	return lbl;
}


string isreg(string str){
	/* binary search on "regs" */
	
	int low = 0, high = REGSIZE - 1; /* the limits of the array */
	string reg = NULL; /* the register with the same name */
	int i;
	char lc; /* the character after the first field in the line (EOS or BLK) */
	
	/* scan for the first character after the first field */
	for (i = 0; str[i] != BLK && str[i] != EOS && str[i] != COM; i++);
	
	/* replace the character with EOS */
	lc = str[i];
	str[i] = EOS;
	
	/* binary search on regs */
	while (low <= high && reg == NULL){
		/* the middle of the searched area */
		int mid = low + (high - low)/2; /* a form of averag that doesn't go beyong high in the calculation (to prevent runtime errors) */
		int cmp = strcmp(str, regs[mid]); /* the relation between the strings (<,>,=) */
		
		if (cmp < 0){
			high = mid - 1;
		} else if (0 < cmp){
			low = mid + 1;
		} else {
			reg = regs[mid];
		}
	}
	
	str[i] = lc; /* binary search on insts */
	
	return reg;
}


int numoffds(string ptr){
	int ctr = 0; /* counter for the fields */
	char pc = BLK; /* the previous character - to count one more */
	
	/* scan the string */
	while (*ptr != EOS){
		/* check if it is a start of a new field */
		if (pc == BLK && *ptr != BLK){
			ctr++; /* count a new field */
		}
		
		pc = *ptr++; /* move to the next character and remember the current one*/
	}
	
	return ctr;
}


int sizeoffd(string str){
	int i;
	
	/* skip characters until BLK or EOS or COM */
	for (i = 0; str[i] != BLK && str[i] != EOS && str[i] != COM; i++);
	
	return i;
}


string dupl(string str){
	string nstr = malloc((sizeoffd(str) + 1) * sizeof(char)); /* allocate space for the new string */
	char *ptr = nstr; /* pointer to set the charatcres of the new string */
	
	/* check the allocation */
	isnull(nstr, NULL);
	
	/* copy-paste all the characters of the string */
	while ((*ptr = *str) != EOS && *ptr != BLK ) {ptr++; str++;}
	
	*ptr = EOS; /* sign the end of the string */
	
	return nstr;
}


string duplw(string str){
	/* allocate space for the new string (if the string is NULL, allocate memory only for the EOS character) */
	string nstr = malloc((str == NULL ? 1 : (WORDSIZE + 1)) * sizeof(char)); 
	int i = 0;
	
	/* check the allocation */
	isnull(nstr, NULL);
	
	/* check if the string is not NULL */
	if (str != NULL){
		/* copy-paste all the characters of the string */
		for (; i < WORDSIZE; i++) nstr[i] = str[i];
	}
	
	nstr[i] = EOS; /* sign the end of the string */
	
	return nstr;
}


string duplstr(string str){
	string nstr = malloc((strlen(str) + 1) * sizeof(char)); /* allocate space for the new string */
	string ptr = nstr; /* pointer to set the charatcres of the new string */
	
	/* check the allocation */
	isnull(nstr, NULL);
	
	/* copy-paste all the characters of the string */
	while ((*ptr++ = *str++) != EOS);
	
	return nstr;
}


string skipfd(string str){
	/* look for the end of the field */
	while (*str != EOS && *str != BLK) str++;
	
	/* check if there is another field */
	return *str == BLK ? str + 1 : str; /* if *str is BLK, return pointer to the first character of the second field, otherwise it is pointer to EOS */
}


string itostr(signed num){
	string numstr = (string)malloc((WORDSIZE + 1) * sizeof(char)); /* allocate space for the string */
	int absn = (num < 0) ? -num : num; /* get the absolut value of num */
	int carry = (num < 0) ? 1 : 0; /* the number to add to the next bit (1 in case of negative number) */
	int i;
	
	isnull(numstr, NULL); /* check the allocation */
	
	/* set all the characters in the string */
	for (i = 0; i < WORDSIZE; i++){
		/* set the i-th character (from right to left) of the string to '1' or '0' based on the current bit of the number
		   and flip it if the original number was negative.									*/
		gbw(numstr, i) = (num < 0) ? (('0' + absn % BINARYBASE == '1') ? '0' : '1') : ('0' + absn % BINARYBASE);
		
		/* get read of the last significant bit (without being dependent on how the system represent the number) */
		absn /= BINARYBASE;
		
		/* add 1 to the current bit if the carry is 1, if the bit was 1, the carry stays 1, otherwise it stays 0 */
		if (carry == 1){
			if (gbw(numstr, i) == '1'){
				gbw(numstr, i) = '0';
				carry = 1;
			} else { /* gbw(numstr, i) == '0' */
				gbw(numstr, i) = '1';
				carry = 0;
			}
		}
	}
	
	/* sign the end of the string */
	numstr[WORDSIZE] = EOS;
	
	return numstr;
}

int findadr(string str){
	char *ptr = str; /* pointer to scan the string */
	char lc; /* the last character of the current field */
	char *lcptr; /* pointer to the first charcter after the first field */
	
	/* scan for the first character after the first field */
	while (*ptr != BLK && *ptr != COM && *ptr != EOS) ptr++;

	lcptr = ptr; /* save the position of teh character */
	lc = *ptr; /* save the character */
	*ptr = EOS; /* replace it with EOS */
	
	
	/* check the addressing */
	if (*str == HSH && *(str + 1) != EOS){ /* check for number operand */
		string suff; /* suffix for the convetion test of the number */
		
		/* check if can't convert to integer because of garbage characters */
		strtol(str + 1, &suff, DECBASE);
		
		*lcptr = lc; /* return the character from the start */
		
		/* check if there are garbage characters */
		if (*suff != EOS){
			return ERR_OPD;
		}
		
		return AC0;
	} else if (isreg(str) != NULL){ /* check for register */
		*lcptr = lc; /* return the character from the start */
		return AC3;
	} else { /* label or struct */
		int fn; /* field number */
	
		/* check if it is a valid name of label */
		ptr = str; /* return ptr to the start of the string */
		
		/* label name starts with alphabet chatacrter */
		if (!isalpha(*ptr)){
			*lcptr = lc; /* return the character from the start */
			return ERR_OPD;
		}
		
		/* all the charatcres in label name are alphanumeric */
		while (*ptr != EOS && *ptr != DOT){ 
			if (!isalnum(*ptr++)){
				*lcptr = lc; /* return the character from the start */
				return ERR_OPD;
			}
		}
		
		/* chekc if the string has only alphanumeric characters and the first is alphabet - it is a label */
		if (*ptr == EOS){
			*lcptr = lc; /* return the character from the start */
			return AC1;
		}
		
		/* as a dot - struct field */
		ptr++; /* point to the field num */
		
		/* check if the number of the field is legal - assume the number of fields in "struct" is onw digit */
		if (0 < (fn = atoi(ptr)) && fn <= GSTCFN && ptr[1] == EOS){
			*lcptr = lc; /* return the character from the start */
			return AC2;
		}
	
		*lcptr = lc; /* return the character from the start */
		return ERR_OPD;
	}
}


int splitline(string line, void **command, string **args, int *lenargs){
	char *ptrl = line; /* pointer to scan the line */
	int endf = 0; /* flag to sign that the end of the line aready have been reached */
	*lenargs = 0; /* reset the counter of the arguments */
	*args = NULL;
	
	/* scan first field of the line */
	while (*ptrl != EOS && *ptrl != BLK){
		if (*ptrl++ == COM){ /* check for illegal comma */
			return ERR_COM_IL;
		}
	}
	
	/* sign the end of the instruction (the first word) */
	if (*ptrl != EOS){
		*ptrl++ = EOS;
	} else { /* (*ptrl == EOS) */
		endf++; /* change the flag to remember that the end of the line have been reached */
	}
	
	/* check if there is an illegal comma between the instruction and the first argument */
	while (*ptrl == COM || *ptrl == BLK){
		if (*ptrl++ == COM){
			return ERR_COM_IL;
		}
	}
	
	/* allocating space for the first argument */
	if (*ptrl != EOS){
		*args = (string *) malloc(sizeof(string));
		
		/* check the allocation */
		if (*args == NULL){
			return ERR_MEM;
		}
	} else { /* (*ptrl == EOS) */
		endf++; /* change the flag to remember that the end of the line has been reached */
	}
	
	/* scanning the line for arguments until the end of the line */
	while (!endf){
		/* allocate space for another argument in args */
		string *temp = (string *) realloc(*args, ++(*lenargs) * sizeof(string));
				
		/* check the allocation */
		if (temp == NULL){
			return ERR_MEM;
		}
		
		*args = temp;
		
		(*args)[*lenargs - 1] = ptrl; /* set the current last argument in args */
		
		/* check if the current argument is a string */
		if (*ptrl == QUM){
			ptrl++; /* move to the first character of the string */
			
			/* skip all the characters of the string */
			while(*ptrl != QUM && *ptrl != EOS) ptrl++;
			
			/* check if found the end of the string wasn't found */
			if (*ptrl != QUM){
				return ERR_OPD; /* the string wasn't closed properly */
			}
			
			ptrl++; /* check the character after the QUM */
			if (*ptrl != BLK && *ptrl != COM && *ptrl != EOS){
				return ERR_OPD;
			}
		} else { /* regular argument - not string*/
			/* skip all the characters of the argument */
			while(*ptrl != BLK && *ptrl != COM && *ptrl != EOS){
				ptrl++;
			}
		}
		
		
		/* check the next character after the argument (*ptrl is BLK or COM or EOS) */
		if (*ptrl == EOS){ /* reached end of line */
			endf++;
		} else if (*ptrl == BLK){
			/* sign the end of the argument */
			*ptrl++ = EOS;
			
			/* skip all the blanks */
			while (*ptrl == BLK){
				ptrl++;
			}
			
			/* check the current character (in a normal case it should be a comma or EOS) */
			if (*ptrl == EOS){ /* reached end of line */
				endf++;
			} else if (*ptrl != COM){ /* some other character */
				return ERR_COM_MI;
			}
		}
		
		/* now the character is COM or EOS */
		
		if (*ptrl == COM){
			*ptrl++ = EOS; /* sign end of argument (won't impact anything if the argument already ended) */
		
			/* skip all the blanks */
			while (*ptrl == BLK) {
				ptrl++;
			}
		
			/* in a normal case, *strl should be now the start of a new argument (a character different from COM and EOS) */
			if (*ptrl == COM){ /* check consecutive commas */
				return ERR_COM_MU;
			} else if (*ptrl == EOS){ /* the line ended instead of having a next argument */
				return ERR_LINE_EXR; /* the comma is an estraneous text */
			}
		}
	}
	
	/* search for the instruction (at the end of the first word there is EOS) */
	*command = (void*)findinst(line);
	
	/* check if the instruction wasn't found */
	if (*command == NULL){
		/* check if it is a guide */
		if (*line == DOT && (*command = (void *)findguide(line + 1)) != NULL){
			return GTYP;
		}
		
		/* not an instruction or guide */
		return ERR_OPT;
	}
	
	return ITYP;
}


void freeall(void *ptr, ...){
	va_list argptr;
	va_start(argptr, ptr);
	
	/* scan the arguments until NULL */
	while (ptr != NULL) {
		free(ptr); /* free the pointer */
		ptr = va_arg(argptr, void *); /* continue to the next pointer */
	}
	
	va_end(argptr);
	return;
}


int fcloseall(FILE *ptr, ...){
	char status = SUCC; /* fclose return EOF if couldn't close the file and zero (which is equal to SUCC) overwise */
	
	va_list argptr;
	va_start(argptr, ptr);
	
	/* scan the arguments until NULL */
	while (ptr != NULL) {
		/* check if all the previous files closed successfully */
		if (status == SUCC){
			status = fclose(ptr); /* return 0 (which is equal to SUCC) in when succeded */
		} else {
			fclose(ptr);
		}
		
		/* continue to the next argument */
		ptr = va_arg(argptr, FILE *);
	}
	
	va_end(argptr);
	
	/* check if succeded to close all the files */
	return status == EOF ? ERR_FCE : SUCC;
}


int powr(int x, int y){
	int ans = 1; /* the resault */
	while (y-- != 0) ans *= x; /* mutiply the "ans" by x, y times */
	return ans;
}


string convert32(string inputNum){
	/* WORDSIZE / BASE2TO32 is the number of 32 base characters needed for WORDSIZE */
	char hw[BASE2TO32 + 1] = {EOS}; /* bits of inputNum equivalent to one 32 base character */
	int num; /* the number represented by the current bits */
	int len = WORDSIZE / BASE2TO32; /* numer of characters in the new string (without EOS) */
	int i;
	
	/* allocating memory for the the converted number */
	string conv = (string) malloc((len + 1) * sizeof(char));
	
	isnull(conv, NULL); /* check the allocataion */
	
	/* scan the "inputNum" */
	for (i = 0; i < len; i++){
		strncat(hw, inputNum + i * BASE2TO32, BASE2TO32); /* put the part of the inputNum in hw */
		num = (int) strtol(hw, NULL, BINARYBASE); /* convert the part to int */
		conv[i] = baseChars[num]; /* put the equivelant 32 character in the string */
		
		*hw = EOS; /* reset the array */
	}
	
	conv[i] = EOS; /* sign the end of the string */
	
	return conv;
}

