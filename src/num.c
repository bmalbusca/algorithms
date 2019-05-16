#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"files.h"







/**********************************
 * 	Main Function
 **********************************/

int main(int argc, char * argv[]){
	
	/* Strings for output/input name files */ 
	char * name_out = NULL;
	
	FILE * fpin = NULL, *fpout = NULL;    
		
	/* Problem */
	matrix_data * p1 = prob_creation();


	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		exit(0);
	}

	
	fpin = fopen ( argv[1], "r" );
	if ( fpin == NULL ) {
		fprintf ( stderr, "ERROR: cannot open file '%s'\n", argv[1]);
		free_data(p1);
		exit (0 );
	}
	

 name_out = file_name_out(argv[1]);

	if(name_out == NULL){
		/*printf("Not a valid namefile\n");*/
		free_data(p1);
		fclose(fpin);
		return 0;
	}
 	
	fpout = fopen(name_out,"w+");
	run_file(fpin,argv[1],p1,fpout,name_out);	
        free_data(p1);  
        free(name_out); 
	
	fclose(fpin);
	fclose(fpout);
	
	return 0;
}
