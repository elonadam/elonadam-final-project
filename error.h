#ifndef _ERROR
#define _ERROR

#define SUCC			0	/* no error */

/* fatal errors, need to stop the program */
#define ERR_MEM			1	/* there was a problem in memory allocation */
#define ERR_FOE			2	/* can't open file */
#define ERR_PRINT		3	/* couldn't print to a file */
#define ERR_FCE			4	/* couldn't close a file */

/* errors in the code provided by the user */
#define ERR_COM_IL		5	/* illegal comma (comma somewhere before the first operand) */
#define ERR_COM_MI		6	/* missing comma between operands */
#define ERR_COM_MU		7	/* multiple consecutive commas */
#define ERR_LINE_EXR	8	/* the line is too long */
#define ERR_LINE_OLBL	9	/* there is only label in the line */
#define ERR_OPD			10	/* illegal operand */
#define ERR_OPT			11	/* unknown operation (not instruction or guide) */
#define ERR_ARGS_NUM	12	/* invalid number of operands */
#define ERR_ARGS_LBL	13	/* unknown label in operand */
#define ERR_ARGS_STC	14	/* label isn't struct */
#define ERR_LBL			15	/* the label already exists */
#define ERR_ENT_LBL		16	/* external label can't be entry */
#define ERR_FULL_MEM	17	/* reached the end of the allowed memory (MEM_END) */
#define ERR_LMT_DAN		18	/*number too big or small for operand numbers */
#define ERR_LMT_GUN		19	/*number too big or small for guide numbers */


/*	report an error according the error code given to the function.
	print explanation about the error to the standard output with the
	number of the line (the line number won't be present in fatal errors).
	SUCC is not an error.													*/
void report(int error, int line);

#endif
