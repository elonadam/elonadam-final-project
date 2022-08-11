#include <string.h>
#include "guides.h"

guide *findguide(string str){
	int low = 0, high = NUMOG - 1; /* the limits of the array */
	guide *gd = NULL; /* the guide with the same name */
	int i;
	char lc; /* the character after the first field in the line (EOS or BLK) */
	
	for (i = 0; str[i] != BLK && str[i] != EOS; i++); /* scan for the first character after the first field */
	
	/* replace the character with EOS */
	lc = str[i];
	str[i] = EOS;
	
	/* binary search on guids */
	while (low <= high && gd == NULL){
		/* the middle of the searched area */
		int mid = low + (high - low)/2; /* a form of averag that doesn't go beyong high in the calculation (to prevent runtime errors) */
		int cmp = strcmp(str, guids[mid].name); /* the relation between the strings (<,>,=) */
		
		if (cmp < 0){
			high = mid - 1;
		} else if (0 < cmp){
			low = mid + 1;
		} else {
			gd = guids + mid;
		}
	}
	
	str[i] = lc; /* return the original character */
	
	return gd;
}
