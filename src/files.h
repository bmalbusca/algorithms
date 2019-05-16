#ifndef FILES
#define FILES

/* Data abstraction - data structure */ 
typedef struct _matrix_def matrix_data;





/* Function prototypes */
matrix_data * prob_creation(void);
void  free_data(matrix_data * p);
void free_matrix(matrix_data *p );
void  run_file(FILE * ptr, char * namefile ,matrix_data  *  data,FILE * ptw, char * name_out );
void  write_file(FILE *ptr, char * namefile, matrix_data * data);
char * file_name_out(char * string);




#endif 
