#include <string.h>
#include "instructs.h"


instruct *findinst(string str){
	int low = 0, high = INSTSIZE - 1; /* the limits of the array */
	instruct *inst = NULL; /* the instruction with the same name */
	int i;
	char lc; /* the character after the first field in the line (EOS or BLK) */
	
	for (i = 0; str[i] != BLK && str[i] != EOS && str[i] != COM; i++); /* scan for the first character after the first field */
	
	/* replace the character with EOS */
	lc = str[i];
	str[i] = EOS;
	
	/* binary search on insts */
	while (low <= high && inst == NULL){
		/* the middle of the searched area */
		int mid = low + (high - low)/2; /* a form of averag that doesn't go beyong high in the calculation (to prevent runtime errors) */
		int cmp = strcmp(str, insts[mid].name); /* the relation between the strings (<,>,=) */
		
		if (cmp < 0){
			high = mid - 1;
		} else if (0 < cmp){
			low = mid + 1;
		} else {
			inst = insts + mid;
		}
	}
	
	str[i] = lc; /* return the original character */
	
	return inst;
}
