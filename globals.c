/* arrays for the keywords and base 32 */

#include "guides.h"
#include "instructs.h"
#include "io.h"

/* array of the guides (guide is defined in guides.h) */
guide guids[] =	{
					{"data"		,	1	,	GNM	,	GALL		,	INTTYP	,	{GNDEF	,	GNDEF }	},
					{"entry"	,	1	,	1	,	GENT		,	LBLTYP	,	{GNDEF	,	GNDEF }	},
					{"extern"	,	1	,	1	,	GEXT		,	LBLTYP	,	{GNDEF	,	GNDEF }	},
					{"string"	,	1	,	1	,	GALL		,	STRTYP	,	{GNDEF	,	GNDEF }	},
					{"struct"	,	2	,	2	,	GALL+GSTC	,	GNDEF	,	{INTTYP	,	STRTYP}	},
				};


/* array of the instructions (instruct is defined in instructs.h) */
instruct insts[] =	{
						{"add"	,	2	,	A0+A1+A2+A3	,	A1+A2+A3	},
						{"bne"	,	10	,	AN			,	A1+A2+A3	},
						{"clr"	,	5	,	AN			,	A1+A2+A3	},
						{"cmp"	,	1	,	A0+A1+A2+A3	,	A0+A1+A2+A3	},
						{"dec"	,	8	,	AN			,	A1+A2+A3	},
						{"get"	,	11	,	AN			,	A1+A2+A3	},
						{"hlt"	,	15	,	AN			,	AN			},
						{"inc"	,	7	,	AN			,	A1+A2+A3	},
						{"jmp"	,	9	,	AN			,	A1+A2+A3	},
						{"jst"	,	13	,	AN			,	A1+A2+A3	},
						{"lea"	,	6	,	A1+A2		,	A1+A2+A3	},
						{"mov"	,	0	,	A0+A1+A2+A3	,	A1+A2+A3	},
						{"not"	,	4	,	AN			,	A1+A2+A3	},
						{"prn"	,	12	,	AN			,	A0+A1+A2+A3	},
						{"rts"	,	14	,	AN			,	AN			},
						{"sub"	,	3	,	A0+A1+A2+A3	,	A1+A2+A3	}
					};


/* array of the registers' names (string is defiend in "io.h") */
string regs[] = {"r0","r1","r2","r3","r4","r5","r6","r7"};

/* the charaters for the 32 base */
char baseChars[] = {'!','@','#','$','%','^','&','*','<','>','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v'};
