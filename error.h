/*
	the errors that are handeld by the functions and a function to 
	report the errros to the to the standard output.
*/

#ifndef _ERROR
#define _ERROR

#define SUCC			0	/* no error */

/* fatal errors, need to stop the program */
#define ERR_MEM			1	/* there was a problem in memory allocation */
#define ERR_FOE			2	/* can't open file */
#define ERR_PRINT		3	/* couldn't print to a file */
#define ERR_FCE			4	/* couldn't close a file */

/* errors in the code provided by the user */
#define ERR_FULL_MEM	5	/* reached the end of the allowed memory (MEM_END) */
#define ERR_COM_IL		6	/* illegal comma (comma somewhere before the first operand) */
#define ERR_COM_MI		7	/* missing comma between operands */
#define ERR_COM_MU		8	/* multiple consecutive commas */
#define ERR_LINE_EXR	9	/* the line is too long */
#define ERR_LINE_OLBL	10	/* there is only label declaration in the line */
#define ERR_OPD			11	/* illegal operand */
#define ERR_OPT			12	/* unknown operation (not instruction or guide) */
#define ERR_ARGS_NUM	13	/* invalid number of operands */
#define ERR_ARGS_LBL	14	/* unknown label in operand */
#define ERR_ARGS_STC	15	/* label isn't struct */
#define ERR_ENT_LBL		16	/* external label can't be entry */
#define ERR_LMT_DAN		17	/* number too big or small for operand numbers */
#define ERR_LMT_GUN		18	/* number too big or small for guide numbers */
#define ERR_LBL			19	/* the label already exists */
#define ERR_LBL_DEC		20	/* invalid label name in declaration */
#define ERR_LBL_LEN		21	/* Label name in label declaration is too long */


/*	report an error according the error code given to the function.
	print explanation about the error to the standard output with the
	number of the line (the line number won't be present in fatal errors).
	SUCC is not an error.													*/
void report(int error, int line);

#endif
