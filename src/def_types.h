#define N_VARS 6
#define N_NEIGHBOURS 8
#define OPT_OK 1
#define INVALID -1
#define VISITED 1



/* Input and output extension files */
#define OUT_EXTEN ".path"
#define INPUT_EXTEN ".puz"


/* *
 *	ADDR(a) -  Cast operator (int* to void*)
 * */

#define ADDR(a) ((void *)&(a))

/* *
 *	CHANGE_BY_ADDR(a) - Acess to data by address ( Casting  void* to int* )
 *
 * */

#define ADDR_TO_VAL(a) *((int *)(a))



#define CAST_TO_PTO(a) (void *)(a)

/* *  	
 *  	 C standard says that all object pointers must be the same size, so void * and int * cannot be different sizE *       BUT Works with all types of pointers
 * */

typedef void * pto ;	/* pointer to - pt */




