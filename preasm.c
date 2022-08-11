#include <stdlib.h>
#include <string.h>
#include "io.h"
#include "supfuncs.h"
#include "table.h"
#include "preasm.h"
#include "error.h"

int pre(char *path){
	FILE *tg, *sc; /* the source (.as) and target (.am) files */
	string fn; /* the name of the files (with ) */
	string line; /* the current line of the source file */
	int len; /* the length of the line */
	int ln = 0; /*line number*/
	char lc = NLN; /* the last character in the line */
	char plc; /* the last character of the previous line */
	table tab; /* the macro table */
	
	/* adds the extension of the input file */
	if ((fn = addstr(path, EAS)) == NULL){
		report(ERR_MEM, ln);
		return ERR_MEM;
	}
	
	/* open the input file on read mode */
	if ((sc = opread(fn)) == NULL){
		free(fn);
		report(ERR_FOE, ln);
		return ERR_FOE;
	}
	free(fn);
	
	/* adds the extension of the output file */
	if ((fn = addstr(path, EAM)) == NULL){
		fclose(sc);
		report(ERR_MEM, ln);
		return ERR_MEM;
	}
	
	if ((tg = opwrite(fn)) == NULL){
		free(fn);
		fclose(sc);
		report(ERR_FOE, ln);
		return ERR_FOE;
	}
	free(fn);
	
	
	if ((tab = createtab()) == NULL){
		fcloseall(sc, tg, NULL);
		report(ERR_MEM, ln);
		return ERR_MEM;
	}
	
	
	while ((plc = lc) != EOF && (line = readline(sc, &lc, &len)) != NULL){
		string name; /* the name of the declared macro */
		int endflag = 0; /* flag to remember if "endmacro" was found */
		string content = NULL; /* the content of a macro */
		ln++;

		/* check if the line is a comment */
		if (*line == SCO){
			free(line);
			continue;
		}
		
		/* check if the line is too long (more than MAXLEN) */
		if (MAXLEN < len){
			free(line);
			report(ERR_LINE_EXR, ln);
			continue;
		}
		
		/* check if it is not a macro declaration */
		if ((name = ismacrodec(tab, line)) == NULL){
			/*	check if there is only one field = could be an existing macro's name
				check if the function found a matching macro 							*/
			if (numoffds(line) == 1 && (content = getcontent(tab, line)) != NULL){ 
				if(writeline(tg, content) == ERR_PRINT){ /* couldn't print to the file */
					fcloseall(sc, tg, NULL);
					freetab(tab);
					report(ERR_PRINT, ln);
					return ERR_PRINT;
				}
			} else { /* not a macro or a refference to a macro */
				if(writeline(tg, line) == ERR_PRINT){/*couldn't print to the file*/
					fcloseall(sc, tg, NULL);
					freetab(tab);
					report(ERR_PRINT, ln);
					return ERR_PRINT;
				}
			}
			
			free(line);
			continue;
		}
		
		
		/* according the assinment, it is guaranteed that there is a line of "endmacro" */
		/* reading lines and storing them until "endmacro" is found */
		while (!endflag){
			string macroline = readline(sc, &lc, &len);
			
			if (macroline == NULL){
				fcloseall(sc, tg, NULL);
				freetab(tab);
				free(line);
				report(ERR_MEM, ln);
				return ERR_MEM;
			}
			
			
			if (MAXLEN < len){
				free(macroline);
				report(ERR_LINE_EXR, ln);
				continue;
			}
			
			/*	if this really is "endmacro", those are the only characters in the line because
				the white characters before and after it are deleted.							 */
			if (!strcmp(macroline, EMAC)){ /* strcmp return 0 only when the strings are equal */
				free(macroline);
				endflag = !endflag; /* found endmacro */
				
			} else {
				/* "addfree" is a macro from "supfuncs.h" */
				/* adds the new line to the macro's content */
				if (content != NULL){
					addfree(content, "\n");
				}
				addfree(content, macroline);
				free(macroline);
			}
		}
		
		
		/* finished reading all the content of the new macro */
		
		/* adding the macro to the table */
		if (addtotab(tab, name, content) == ERR_MEM){ /* check if there was a memory problem */
			fcloseall(sc, tg, NULL);
			freetab(tab);
			report(ERR_MEM, ln);
			return ERR_MEM;
		}
		
		free(line);
	}
	
	freetab(tab);
	
	if (fcloseall(sc, tg, NULL) == EOF){
		report(ERR_FCE, ln);
		return ERR_FCE;
	}
	
	return SUCC;
}
