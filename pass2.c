#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io.h"
#include "supfuncs.h"
#include "memoryList.h"
#include "guides.h"
#include "error.h"
#include "table.h"
#include "instructs.h"
#include "pass2.h"


int pass2(char *path, int status, table tab, word *head){
	FILE *src, *obj, *ent, *ext; /* pointers to source file and destination files */
	string fn; /* the name of the files */
	char lc = NLN; /* the last character of the current file */
	char plc; /* the previous last character*/
	string line; /* current line */
	int ln = 0; /* line counter */
	int ic = 0, dc = 0; /* instruction and data counters */
	int i;
	int res; /* returned value from functions that report about more than one error */
	word *wordnode = head; /* pointer to edit the word list */
	lblword *enthead; /* list for entry labels */
	lblword *exthead; /* list for extern labels */
	
	enthead = addnextlbl(NULL, NULL, NULL); /* create dummy head for the list */
	exthead = addnextlbl(NULL, NULL, NULL); /* create dummy head for the list */
	
	/* check if the lists were created */
	if (enthead == NULL || exthead == NULL){
		freeLblList(enthead);
		freeLblList(exthead);
		freeMemList(head);
		freetab(tab);
		report(ERR_MEM, ln);
		return ERR_MEM;
	}
	
	/* create the name of the source file */
	if ((fn = addstr(path, EAM)) == NULL){
		freeLblList(enthead);
		freeLblList(exthead);
		freeMemList(head);
		freetab(tab);
		report(ERR_MEM, ln);
		return ERR_MEM;
	}
	
	/* open the source file */
	if ((src = opread(fn)) == NULL){
		freeLblList(enthead);
		freeLblList(exthead);
		freeMemList(head);
		freetab(tab);
		free(fn);
		report(ERR_FOE, ln);
		return ERR_FOE;
	}
	
	free(fn);
	
	/* scan the file's lines */
	while ((plc = lc) != EOF && (line = readline(src, &lc, NULL)) != NULL){ /* read line from source file */
		string offset = line; /* to skip the part of the line with label */
		string *args; /* the operands of the current line */
		int lenargs; /* number of operands */
		string lbl; /* string for the label of the current line */
		int lbllen; /* the length of the label declaration */
		int adrtg = ACN; /* the addressing method of the target operand */
		int adrsc = ACN; /* the addressing methos of the source operand */
		instruct *ip; /* instruct pointer */
		int opNum; /* the number of operands requied by the current instruction */
		
		ln++; /* count the current line */
		
		/* check if the line is empty */
		if (*line == EOS){
			free(line);
			continue;
		}
		
		
		/* check if there is a label at the start of the line */
		lbl = islbl(line);
		lbllen = sizeoffd(line);
		
		if (lbl != NULL){ /* check if the label name is valid */
			offset = skipfd(line); /* skip the label */
			
			/* if the label name is too long, forget it and report */
			if(LBLLENMAX < lbllen){
				/* reported in pass1 */
				status = ERR_LBL_LEN;
				free(lbl);
				lbl = NULL;
			}
			
			/* check if there is only label in the line */
			if (*offset == EOS){
				freeall(line, lbl, NULL);
				/* reported in pass1 */
				status = ERR_LINE_OLBL;
				continue; /* continue to the next line */
			}
			
		} else if (line[lbllen - 1] == COL){ /* check if the label name wasn't valid */
			offset = skipfd(line); /* skip the label */
			
			/* reported in pass1 */
			status = ERR_LINE_OLBL;
			
			/* check if there is only label in the line */
			if (*offset == EOS){
				free(line);
				/* reported in pass1 */
				status = ERR_LINE_OLBL;
				continue; /* continue to the next line */
			}
		}
		
		
		/* split the line to several strings for further analyze */
		if((res = splitline(offset, (void**)(&ip), &args, &lenargs)) != GTYP && res != ITYP) {/* if res is not GTYP or ITYP there was an error */
			freeall(line, args, NULL);
			
			/* check which error occurred */
			if (res == ERR_MEM){
				freeLblList(enthead);
				freeLblList(exthead);
				freeMemList(head);
				freetab(tab);
				fclose(src);
				report(ERR_MEM, ln);
				return ERR_MEM;
			} else {
				/* the error is reported in "pass1" */
				status = res;
				continue; /* move to the next line */
			}
		}
		
		if(res == GTYP){ /* check if the operation is guide */
			guide *gp = (guide *)ip; /* covert ip to pointer to guide */
			
			/* check if the guide is not entry */
			if(gp -> kind != GENT){
				freeall(line, args, NULL);
				continue; /* move to the next line */
			}
			
			/* check the num of operands */
			if(!((gp->opmin) <= lenargs && (lenargs <= (gp->opmax) || (gp->opmax) == GNM))){ 
				freeall(line, args, NULL);
				report(status = ERR_ARGS_NUM, ln);
				continue; /* move to the next line */
			}
			
			/* scan the operands and add them to the entry list */
			for(i = 0; i < lenargs; i++){
				int kind; /* the kind of the label */
				char address[WORDSIZE]; /* the address of the entried label */
				string lblw; /* the label's word */
				int j;
				
				/* check if the label exists and the kind is not GEXT (extern)*/
				if((kind = getkind(tab, args[i])) == NOTFOUND){ /* if the label exists */
					report(status = ERR_ARGS_LBL, ln);
					continue; /* move to the next operand */
				} else if(kind == GEXT) { /* if the kind is extern */
					report(status = ERR_ENT_LBL, ln);
					continue; /* move to the next operand */
				}
				
				/* get the label's word */
				lblw = getcontent(tab, args[i]);
				
				/* get the label's address */
				
				for (j = 0; j < ADRSTRT; j++){
					address[j] = '0'; /* set to zero the unused chaacters */
				}
				for (j = 0; j < ADRLEN; j++){
					gbw(address, j) = gbw(lblw, j + ADRSTRT); /* shift the bits of the address to the right */
				}
				
				/* add the label to the entry list */
				if(addnextlbl(enthead, args[i], address) == NULL){
					freeall(line, args, NULL);
					freeLblList(enthead);
					freeLblList(exthead);
					freeMemList(head);
					freetab(tab);
					fclose(src);
					report(ERR_MEM, ln);
					return ERR_MEM;
				}
			}
			
			freeall(line, args, NULL);
			continue; /* move to the next line */
		}/* end of guide check */
		
		opcount(opNum,ip); /* count number of operands that ip require */
		if (opNum != lenargs){ /* check number of operands */
			/* error reported in "pass1" */
			status = ERR_ARGS_NUM;
			continue; /* move to the next line */
		}
		
		/* check the addressing methods */
		i = 0;
		switch (lenargs){ /* check the number of operands */
			case TWOARGS: /* there are two arguments - the first is the source and the second is the target */
				adrsc = findadr(args[i++]);
			case ONEARGS: /* there is one arguments - which is the target */
				adrtg = findadr(args[i++]);
			break;
		} /* adrtg will remain ACN if there are no operands */
		
		/* check if the syntax of the operands is legal */
		if (adrsc == ERR_OPD || adrtg == ERR_OPD){
			freeall(line, args, NULL);
			report(status = ERR_OPD, ln);
			continue; /* move to the next line */
		}
		
		/* write the addressing method in the operation word */
		if (wordnode != NULL){ /* make sure it is on the operation word (it could be NULL if there was a previous error) */
			for(i = 0; i < ADSLEN; i++) { 
				gbw(wordnode->data, i + ADSTGST) = getbit((adrtg == ACN ? AC0 : adrtg), i) + '0'; /* write the addressing method of the target */
				gbw(wordnode->data, i + ADSSCST) = getbit((adrsc == ACN ? AC0 : adrsc), i) + '0'; /* write the addressing method of the source */
			}
		}
			
		/* check if the addressing method match the instruction (if there is no operand the check does nothing) */
		if ((adrtg != ACN && (ip->optg & powr(BINARYBASE, adrtg)) != powr(BINARYBASE, adrtg)) 
		 || (adrsc != ACN && (ip->opsc & powr(BINARYBASE, adrsc)) != powr(BINARYBASE, adrsc))) {
			report(status = ERR_OPD, ln);
			freeall(line, args, NULL);
			continue; /* move to the next line */
		}
		
		ic++; /* count the operation word */
		
		/* check if both operands are register (if there is less then two operands, at least one of them will be ACN) */
		if (adrtg == AC3 && adrsc == AC3){
			char memword[WORDSIZE + 1]; /* the registers' string word */
			int srcreg = atoi(fstArg + 1); /* the number of the source register (the first character of the register is 'r') */
			int trgtreg = atoi(secArg + 1); /* the number of the target register (the first character of the register is 'r') */
			word *tmp; /* temporery word to check ERR_MEM */
			
			memword[WORDSIZE] = EOS; /* sign the end of the string */
			
			/* writes the numbers (in binary) of the registers in the new word */
			for(i = 0; i < REGLEN; i++ ) {
				gbw(memword, i + REGTGST) = getbit(trgtreg, i) + '0'; /* write the target register */
				gbw(memword, i + REGSCST) = getbit(srcreg, i) + '0' ; /* write the source register */
			}
			
			opcA(memword); /* set the ARE of the word */
			
			/* add the word to the memory list */
			if (wordnode != NULL){ /* make sure it is on the operation word (it could be NULL if there was a previous error) */
				if ((tmp = addnext(wordnode, memword)) == NULL){
					freeall(line, args, NULL);
					freeLblList(enthead);
					freeLblList(exthead);
					freeMemList(head);
					freetab(tab);
					fclose(src);
					report(ERR_MEM, ln);
					return ERR_MEM;
				}
				wordnode = tmp; /* move to the new node */
				ic++; /* count the new word */
			}
		} else { /* all other cases that aren't 2 registers as arguments*/
			int op; /* numeric operands (after cast to int) */
			int kind; /* the kind of label operand */
			int reg; /* the number of a register operand (for example  1  is the number of the register "r1") */
			char *ptr; /* pointer to the character (representing number) of the field in a struct operand */
			word *tmp; /* temporery word for ERR_MEM checks */
			string cop; /* the string words that should be added to the memory list */
			
			/* check the adressing method of the first operand */ 
			switch(lenargs == TWOARGS ? adrsc : adrtg){ /* if there is one operand it is the target, if there are two it is the source */
				case AC0: /* the operand is a number */
					op = atoi(fstArg + 1); /* cast the number of the operand (the first character is HSH)*/
					
					/* check if the number is too big or small */
					if (op < MINOPNUM || MAXOPNUM < op){
						freeall(line, args, NULL);
						report(status = ERR_LMT_DAN, ln);
						continue; /* move to the next line */
					}
					
					/* convert the number to binary representation */
					if((cop = itostr(op)) == NULL){
						freeall(line, args, NULL);
						freeLblList(enthead);
						freeLblList(exthead);
						freeMemList(head);
						freetab(tab);
						fclose(src);
						report(ERR_MEM, ln);
						return ERR_MEM;
					}
					
					movel2(cop); /* move to the left to set the ARE */
					opcA(cop); /* set the ARE */
					
					/* add the operand to the list */
					if (wordnode != NULL){ /* make sure it is on the operation word (it could be NULL if there was a previous error) */
						if((tmp = addnext(wordnode, cop)) == NULL){
							freeall(line, args, cop, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
						wordnode = tmp; /* change to the new node */
						ic++; /* count the added operand word */
					}
					
					free(cop);
					
					break;
					
				case AC1: /* the operand is a label */
					/* check if label (operand) exists in table */
					if((kind = getkind(tab, fstArg)) == NOTFOUND){ 
						freeall(line, args, NULL);
						report(status = ERR_ARGS_LBL, ln);
						continue; /* move to the next line */
					}
					
					cop = getcontent(tab, fstArg); /* get content of the label */
					
					/* check if the label is external */
					if(kind == GEXT){
						string strIC; /* the place where the reference to the external label is */
						
						opcE(cop); /* set the ARE */
						
						/* cast the position to binary */
						if ((strIC = itostr(MEM_STRT + ic)) == NULL){
							freeall(line, args, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
						
						/* add the label and position to the reference list */
						if(addnextlbl(exthead, fstArg, strIC) == NULL){
							freeall(line, args, strIC, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
						
						free(strIC);
					} else {
						opcR(cop); /* set the ARE */
					}
					
					/* add the operand to the list */
					if (wordnode != NULL) { /* make sure it is on the operation word (it could be NULL if there was a previous error) */
						if((tmp = addnext(wordnode, cop)) == NULL){ /* check if added to list */
							freeall(line, args, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
						wordnode = tmp; /* change to the new node */
						ic++; /* count the added operand word */
					}
					
					break;
					
				case AC2: /* the operand is struct field (should add the address of the label and the number of the field to the list) */
					ptr = strchr(fstArg, DOT); /* return pointer the the first encounter with dot */
					*ptr = EOS; /* replace with end of string sign */
					ptr++; /* move forward by one (to the field's number) */
					
					/* get the kinf of label (if exists) */
					if((kind = getkind(tab, fstArg)) == NOTFOUND){
						freeall(line, args, NULL);
						report(status = ERR_ARGS_LBL, ln);
						continue; /* move to the next line */
					}
					
					/* check if label is not struct or extern */
					if(kind != GSTC && kind != GEXT){ 
						report(status = ERR_ARGS_STC, ln);
						freeall(line, args, NULL);
						continue; /* move to the next line */
					}
					
					cop = getcontent(tab, fstArg); /* get the labels address */
					
					/* check if the label is external */
					if(kind == GEXT){
						string strIC; /* the place where the reference to the external label is */
						
						opcE(cop); /* set the ARE */
						
						/* cast the position to binary */
						if((strIC = itostr(MEM_STRT + ic)) == NULL){
							freeall(line, args, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;	
						}
						
						/* add the label and position to the reference list */
						if (addnextlbl(exthead, fstArg, strIC) == NULL){ /* check if added to the list */
							freeall(line, args, strIC, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}

						free(strIC);
					} else {
						opcR(cop); /* set the ARE */
					}
					
					/* add label to the list */
					if (wordnode != NULL){ /* make sure it is on the operation word (it could be NULL if there was a previous error) */
						if ((tmp = addnext(wordnode, cop)) == NULL){ /* check if added to the list */
							freeall(line, args, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}

						wordnode = tmp; /* change to the next node */
						ic++; /* count the added node */
					}
					
					/* convert to string the field's number */
					if ((cop = itostr(atoi(ptr))) == NULL){
						freeall(line, args, NULL);
						freeLblList(enthead);
						freeLblList(exthead);
						freeMemList(head);
						freetab(tab);
						fclose(src);
						report(ERR_MEM, ln);
						return ERR_MEM;
					}
					
					movel2(cop); /* move to make place for the ARE */
					opcA(cop); /* set teh ARE */
					
					/* add the field's number to the list */
					if (wordnode != NULL){ /* make sure it is on the operation word (it could be NULL if there was a previous error) */
						if ((tmp = addnext(wordnode, cop)) == NULL){
							freeall(line, args, cop, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}

						wordnode = tmp; /* move to the new node */
						ic++; /* count the added node */
					}
					
					free(cop);
					
					break;
					
				case AC3: /* the operand is a register */
					cop = (string)malloc((WORDSIZE + 1) * sizeof(char)); /* allocate new memory for the operand */
					reg = atoi(fstArg + 1); /* get the register's number */
					
					/* check the memory allocate */
					if (cop == NULL){ 
						freeall(line, args, NULL);
						freeLblList(enthead);
						freeLblList(exthead);
						freeMemList(head);
						freetab(tab);
						fclose(src);
						report(ERR_MEM, ln);
						return ERR_MEM;
					}
					
					cop[WORDSIZE] = EOS; /* sign the end of the string */
					
					/* write the register's number to the string */
					for(i = 0; i < WORDSIZE; i++ ) cop[i] = '0'; /* utilized cells to '0' */
					for(i = 0; i < REGLEN; i++ ) { /* write the register number to the operand word */
						/* if there is only one operand, it should be written on the target register.
						   if there are two operands, it should be written to the source register	*/
						gbw(cop, i + (lenargs == TWOARGS ? REGSCST : REGTGST)) = getbit(reg, i) + '0' ;
					}
					
					opcA(cop); /* set the ARE of the operand's word */
					
					/* add the word to the list */
					if (wordnode != NULL){ /* make sure it is on the operation word (it could be NULL if there was a previous error) */
						if ((tmp = addnext(wordnode, cop)) == NULL){
							freeall(line, args, cop, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
						wordnode = tmp; /* change to the new node */
						ic++; /* count the new node */
					}
					
					free(cop);
					
					break;
					
			} /* end of switch for first operand */
			
			/* check the adressing method of the second operand */ 
			switch(lenargs == TWOARGS ? adrtg : ACN){ /* if there are two arguments it is the target, otherwise check no one */
				case AC0: /* the operand is a number */
					op = atoi(secArg + 1); /* cast the number of the operand (the first character is HSH) */
					
					/* check if the number is too big or small */
					if (op < MINOPNUM || MAXOPNUM < op){
						freeall(line, args, NULL);
						report(status = ERR_LMT_DAN, ln);
						continue; /*move to the next line*/
					}
										
					/* convert the number to binary representation */
					if((cop = itostr(op)) == NULL){
						freeall(line, args, NULL);
						freeLblList(enthead);
						freeLblList(exthead);
						freeMemList(head);
						freetab(tab);
						fclose(src);
						report(ERR_MEM, ln);
						return ERR_MEM;
					}
					
					movel2(cop); /* move to the left to set the ARE */
					opcA(cop); /* set the ARE */
					
					/* add the operand to the list */
					if (wordnode != NULL){ /* make sure it is on the operand word (it could be NULL if there was a previous error) */
						if((tmp = addnext(wordnode, cop)) == NULL){
							freeall(line, args, cop, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}

						wordnode = tmp; /* change to the new node */
						ic++; /* count the added operand word */
					}
					
					free(cop);
					
					break;
					
				case AC1: /* the operand is a label */
					/* check if label (operand) exists in table */
					if((kind = getkind(tab, secArg)) == NOTFOUND){ 
						freeall(line, args, NULL);
						report(status = ERR_ARGS_LBL, ln);
						continue; /* move to the next line */
					}
					
					cop = getcontent(tab, secArg); /* get content of the label */
					
					/* check if the label is external */
					if(kind == GEXT){
						string strIC; /* the place where the reference to the external label is */
						
						opcE(cop); /* set the ARE */
						
						/* cast the position to binary */
						if ((strIC = itostr(MEM_STRT + ic)) == NULL){
							freeall(line, args, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
						
						/* add the label and position to the reference list */
						if(addnextlbl(exthead, secArg, strIC) == NULL){
							freeall(line, args, strIC, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
						
						free(strIC);
					} else {
						opcR(cop); /* set the ARE */
					}
					
					/* add the operand to the list */
					if (wordnode != NULL){ /* make sure it is on the operand word (it could be NULL if there was a previous error) */
						if((tmp = addnext(wordnode, cop)) == NULL){ /* check if added to list */
							freeall(line, args, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
						wordnode = tmp; /* change to the new node */
						ic++; /* count the added operand word */
					}
					
					break;
					
				case AC2: /* the operand is struct field (should add the address of the label and the number of the field to the list) */
					ptr = strchr(secArg, DOT); /* return pointer the the first encounter with dot */
					*ptr = EOS; /* replace with end of string sign */
					ptr++; /* move forward by one (to the field's number) */
					
					/* get the kinf of label (if exists) */
					if((kind = getkind(tab, secArg)) == NOTFOUND){
						freeall(line, args, NULL);
						report(status = ERR_ARGS_LBL, ln);
						continue; /* move to the next line */
					}
					
					/* check if label is not struct or extern */
					if(kind != GSTC && kind != GEXT){ 
						report(status = ERR_ARGS_STC, ln);
						freeall(line, args, NULL);
						continue; /* move to the next line */
					}
					
					cop = getcontent(tab, secArg); /* get the labels address */
					
					/* check if the label is external */
					if(kind == GEXT){
						string strIC; /* the place where the reference to the external label is */
						
						opcE(cop) /* set the ARE */
						
						/* cast the position to binary */
						if((strIC = itostr(MEM_STRT + ic)) == NULL){
							freeall(line, args, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;	
						}
						
						/* add the label and position to the reference list */
						if (addnextlbl(exthead, secArg, strIC) == NULL){ /* check if added to the list */
							freeall(line, args, strIC, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}

						free(strIC);
					} else {
						opcR(cop); /* set the ARE */
					}
					
					/* added label to the list */
					if (wordnode != NULL){ /* make sure it is on the operand word (it could be NULL if there was a previous error) */
						if ((tmp = addnext(wordnode, cop)) == NULL){ /* check if added to the list */
							freeall(line, args, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}

						wordnode = tmp; /* change to the next node */
						ic++; /* count the added node */
					}
					
					/* convert to string the field's number */
					if ((cop = itostr(atoi(ptr))) == NULL){
						freeall(line, args, NULL);
						freeLblList(enthead);
						freeLblList(exthead);
						freeMemList(head);
						freetab(tab);
						fclose(src);
						report(ERR_MEM, ln);
						return ERR_MEM;
					}
					
					movel2(cop); /* move to make place for the ARE */
					opcA(cop); /* set teh ARE */
					
					/* add the field's number to the list */
					if (wordnode != NULL){ /* make sure it is on the operand word (it could be NULL if there was a previous error) */
						if ((tmp = addnext(wordnode, cop)) == NULL){
							freeall(line, args, cop, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}

						wordnode = tmp; /* move to the new node */
						ic++; /* count the added node */
					}
					
					free(cop);
					
					break;
					
				case AC3:
					cop = (string)malloc((WORDSIZE + 1) * sizeof(char)); /* allocate new memory for the operand */
					reg = atoi(secArg + 1); /* get the register's number */
					
					/* check the memory allocate */
					if (cop == NULL){
						freeall(line, args, NULL);
						freeLblList(enthead);
						freeLblList(exthead);
						freeMemList(head);
						freetab(tab);
						fclose(src);
						report(ERR_MEM, ln);
						return ERR_MEM;
					}
					
					cop[WORDSIZE] = EOS; /* sign the end of the string */
					
					for(i = 0; i < WORDSIZE; i++ ) cop[i] = '0'; /* utilized cells to '0' */
					for(i = 0; i < REGLEN; i++ ) { /* write the register nimber to the operand word */
						/* write the register's number on the  target register */
						gbw(cop, i + REGTGST) = getbit(reg, i) + '0' ;
					}
					
					opcA(cop); /* set the ARE of the operand's word */
					
					/* add the word to the list */
					if (wordnode != NULL){ /* make sure it is on the operand word (it could be NULL if there was a previous error) */
						if ((tmp = addnext(wordnode, cop)) == NULL){
							freeall(line, args, cop, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freetab(tab);
							fclose(src);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}

						wordnode = tmp; /* change to the new node */
						ic++; /* count the new node */
					}
					
					free(cop);
					
					break;
					
			} /* end of switch for second operand */	
		} /* end of else (the operands' addressings checks)*/
		
		/* move to the next instruction word */
		if (wordnode != NULL){ /* make sure it is on the operand word (it could be NULL if there was a previous error) */
			wordnode = wordnode->next;
		}
		
		freeall(line, args, NULL);
	} /* end of while (scan the source file) */
	
	freetab(tab);
	
	/* close the source file */
	if (fclose(src) == EOF){
		freeLblList(enthead);
		freeLblList(exthead);
		freeMemList(head);
		report(ERR_FCE, ln);
		return ERR_FCE;
	}
	
	/* check if there was an error - not printing to the output file */
	if(status != SUCC){ 
		freeLblList(enthead);
		freeLblList(exthead);
		freeMemList(head);
		return status;
	}
	
	/* update the data-counter */
	if (wordnode != NULL){ /* check if there is any data */
		dc++; /* count the current node */
		while((wordnode = wordnode -> next) != NULL) dc++; /* count the nodes until NULL */
	}
	
	
	/* the name of the object file */
	if ((fn = addstr(path, EOB)) == NULL){
		freeLblList(enthead);
		freeLblList(exthead);
		freeMemList(head);
		report(ERR_MEM, ln);
		return ERR_MEM;
	}
	
	/* create\open the object file */
	if ((obj = opwrite(fn)) == NULL){
		freeLblList(enthead);
		freeLblList(exthead);
		freeMemList(head);
		free(fn);
		report(ERR_FOE, ln);
		return ERR_FOE;
	}
	
	free(fn);
	
	/* print the memory list to the object file */
	if ((res = printMemList(obj, head, ic, dc)) != SUCC){
		fclose(obj);
		freeLblList(enthead);
		freeLblList(exthead);
		freeMemList(head);
		fclose(obj);
		report(res, ln);
		return res;
	}
	
	freeMemList(head);
	
	/* close the object file */
	if (fclose(obj) == EOF){
		freeLblList(enthead);
		freeLblList(exthead);
		report(ERR_FCE, ln);
		return ERR_FCE;
	}
	
	
	/* the name of the entry file */
	if ((fn = addstr(path, ENT)) == NULL){
		freeLblList(enthead);
		freeLblList(exthead);
		report(ERR_MEM ,ln);
		return ERR_MEM;
	}
	
	/* open\create the entry file */
	if ((ent = opwrite(fn)) == NULL){
		freeLblList(enthead);
		freeLblList(exthead);
		free(fn);
		report(ERR_FOE, ln);
		return ERR_FOE;
	}
	
	free(fn);
	
	/* print the entry list to the entry file (enthead->next  to skip the dummy head) */
	if((res = printLblList(ent, enthead->next)) != SUCC){
		freeLblList(enthead);
		freeLblList(exthead);
		fclose(ent);
		report(res, ln);
		return res;
	}
	
	freeLblList(enthead);
	
	/* close the rntry file */
	if (fclose(ent) == EOF){
		freeLblList(exthead);
		report(ERR_FCE, ln);
		return ERR_FCE;
	}
	
	
	/* the name of the extern file */
	if ((fn = addstr(path, EXT)) == NULL){
		freeLblList(exthead);
		report(ERR_MEM ,ln);
		return ERR_MEM;
	}
	
	/* open\create the extern file */
	if ((ext = opwrite(fn)) == NULL){
		free(fn);
		freeLblList(exthead);
		report(ERR_FOE, ln);
		return ERR_FOE;
	}
	
	free(fn);
	
	/* print the extern list to the extern file (exthead->next  to skip the dummy head) */
	if ((res = printLblList(ext, exthead->next)) != SUCC) {
		freeLblList(exthead);
		fclose(ext);
		report(res, ln);
		return res;
	}
	
	freeLblList(exthead);
	
	/* close the extern file */
	if (fclose(ext) == EOF){
		report(ERR_FCE, ln);
		return ERR_FCE;
	} 
	
	
	return SUCC;
} /* end of pass2 */
