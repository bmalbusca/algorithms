#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "files.h"
#include "def_types.h"

#define OUT_EXTEN ".path"
#define INPUT_EXTEN ".puz"


/*
 * Structure used from define a turn (move)
 * ATENCION: This is use a define type, see in def_types.h 
 */ 

typedef struct _turn{

	int  l;
	int  c;
	pto val; /*only saves the address of values */  
	pto * adj; 
}turn;



/*
 * Structure which describes the problem to solve
 * ATENCION: This is a support for a abstract type, see in files.h 
 */ 


struct _matrix_def{

	int lines;
	int columns;
	char option;
	int l0;
	int c0;
	int k;  /* read from file */
	int k_turns;

	int pairs_numbers;
	turn ** path; 
	int ** matrix;
};




/**********************************************
 * FUNCTION PROTOTYPES
 **********************************************/

pto  option_validation( matrix_data * data, FILE * ptr);
pto intial_coordinates(matrix_data *p);
pto  k_validation(matrix_data *p);
void  fill_matrix (FILE * ptr, matrix_data * data);
void  skip_parsing(FILE * ptr, int n_vals);
void allocation_matrix(matrix_data * data);
void find_next_blank_line(FILE *ptr);
void  option_D(matrix_data * data);



void mem_error(pto pointer , char * function){

	if(pointer == NULL){
		printf("*ERROR* Memory allocation failed: %s() \n",function);
		exit(0);

	}

}



/*
 *	FUNCTION:  	prob_creation
 *	ARGS:			
 *	RETURN:		(matrix_data *) 
 *	DESCRIPTION: 	Allocate memory for a structure that will contain data from the problem 
 */ 

matrix_data * prob_creation(void){


	matrix_data * p1 = NULL;

	p1 = (matrix_data *)calloc(1,(sizeof(matrix_data)));

	if(p1 == NULL){
		printf("*ERROR* Memory allocation failed: ");
		exit(0);
	}

	return p1;

}


/*
 *	FUNCTION:  	free_matrix
 *	ARGS:		(matrix_data *)	
 *	RETURN:		 
 *	DESCRIPTION: 	Free matrix allocated to solve the problem
 */ 

void free_matrix(matrix_data *p ){

	int i =0;

	if( p->matrix != NULL ){ 
		for(i =0;i < (p->lines);++i){
			free(p->matrix[i]);
		}
	}

	if (p->matrix != NULL){
		free(p->matrix);
	}
	p->matrix = NULL;
}



/*
 *	FUNCTION:  	free_data
 *	ARGS:		(matrix_data *)	
 *	RETURN:		 
 *	DESCRIPTION: 	Free all memory allocated to solve the problem
 */ 

void  free_data(matrix_data * p){

	/* printf("Free memory process\n"); */
	if( p->matrix != NULL ){ 
		free_matrix(p);
	}
	free(p);
}


/*
 *	FUNCTION:  	file_name_out
 *	ARGS:		(char *)	
 *	RETURN:		(char *)
 *	DESCRIPTION: 	output filename. This function returns a string and you need to make a free after calling this function
 */ 


char * file_name_out(char * string ){

	int str_length = 0, ext_length =0;
	char * token = NULL;
	char * name_out = NULL; 

	/* last occurrence of .puz0 extension */
	token = strrchr(string,'.'); 
	if(strcmp(token,INPUT_EXTEN)==0 ){

		ext_length = strlen(OUT_EXTEN); /* ".path" extension  */
		str_length = strlen(string) - strlen(INPUT_EXTEN) ;

		name_out = (char*)calloc( (str_length + ext_length +1 ), sizeof(char));

		if(name_out == NULL){
			printf("*ERROR* not enough memory available!\n ");
			exit(0);
		}

		/* copy only the interesting part */
		memmove(name_out,(string),(str_length));	        
		/* add extension .adj*/
		strcat(name_out, OUT_EXTEN);

	}  
	return name_out;




}



/*
 *	FUNCTION:  	 initial_coordinates
 *	ARGS:		 matrix_data	
 *	RETURN:		 (void *)
 *	DESCRIPTION: 	 check if coordinates are valid. Invalid case returns NULL, otherwise returns OPT_OK   
 */ 

pto  intial_coordinates(matrix_data *p){

	/* check if the given starting point is valid */
	if( ((p->l0 <= (p->lines)-1 ) && p->l0 >=0) && (p->c0 <= (p->columns)-1 && p->c0 >=0 )){
		return (void *)OPT_OK;
	}

	return NULL;

}



/*
 *	FUNCTION:  	 c_option
 *	ARGS:		 matrix_data	
 *	RETURN:		 (void *)
 *	DESCRIPTION: 	 In case of C option, check if K is a valid number, otherwise returns NULL   
 */ 

pto k_validation(matrix_data *p){

	if(((p->k) >= 1 && (p->k < (((p->lines) * (p->columns)) -1) ))){
		return (void *)OPT_OK;
	}
	return NULL;
}



/*
 *	FUNCTION:  	 skip_parsing
 *	ARGS:		 FILE * , int , int
 *	RETURN:		 FILE *
 *	DESCRIPTION: 	 Skip the matrix reading process 
 */

void  skip_parsing(FILE * ptr, int n_vals){

	int i, fl=0;
	/*fscanf(ptr, "%*[^\n]\n", NULL);*/
	for(i = 0; i < n_vals && fl > -1 && (!feof(ptr)); ++i){
		/* '*' read but ignored */
		fl = fscanf(ptr ,"%*d");

	}

	return; 

}



/*
 *	FUNCTION:  	 find_next_blank_line
 *	ARGS:		 FILE *
 *	RETURN:		 
 *	DESCRIPTION: 	 find the next blank line and ignores all data until find the blank line
 */



void find_next_blank_line(FILE *ptr){

	size_t size_buffer = 0;
	int num_ch_read = 0;
	char * buffer = NULL;

	if(ptr == NULL){ return; }

	/* EOF -> -1 */
	while( (num_ch_read = getline(&buffer, &size_buffer, ptr))!=-1) {

		if( strcmp(buffer,"\n") == 0){  /* num_ch_read == 1 */
			free(buffer);
			return;		/* blank line found */
		}
	}
	// printf("No blank lines\n");
	if(buffer != NULL){
		free(buffer);
	}
}




/*
 *	FUNCTION:  	 move
 *	ARGS:		 matrix_data , int , int
 *	RETURN:		 pt *
 *	DESCRIPTION: 	 Check if the move is valid!In case of failure, returns NULL 
 */


pto * move(int l,int c, int l0, int c0, matrix_data * data){

	if(l > (data->lines -1) || l < 0){
		return NULL;
	}
	if(c > (data->columns -1)  || c < 0){
		return NULL;
	}

	if((l == l0) && (c == c0)){
		return NULL; 
	}

	return (pto)OPT_OK;

}





/*
 *	FUNCTION:  	 check_even
 *	ARGS:		 int
 *	RETURN:		 int
 *	DESCRIPTION: 	 Check if the variable val is even: success returns 0  
 */

int  check_even(int val){
	return val%2;
}



pto even(int val1, int val2, int * val){

	if(check_even(val2)==0){
		*val = val1;				
		return (pto)OPT_OK;
	}

	return NULL;
}




/*
 *	FUNCTION:  	 first_val
 *	ARGS:		 int , matrix_data* , int*
 *	RETURN:		 
 *	DESCRIPTION:	counts the number of valid moves for a even number and at first time, stores the value in matrix_data structure	 
 */


void first_val(int point_val , matrix_data * data, int * cnt ,int *val){

	if(*cnt==0){
		*val = point_val;
		data->k_turns=1;			/* the program found a even number */

	}
	(*cnt)=(*cnt)+1; 						/* helps to find which iterations fails */
}





turn * insert_edge(int l, int c,  pto address_val ){

	turn * edge = NULL;
	edge = (turn *)calloc(1, sizeof(turn));

	if(edge == NULL){
		printf("*ERROR* Memory allocation error: insert_edge() \n");
		exit(0);
	}

	edge->adj = (pto *) calloc(N_NEIGHBOURS+1, sizeof(pto));

	if(edge->adj == NULL){
		printf("*ERROR* Memory allocation error: insert_edge() \n");
		exit(0);
	}

	edge->l = l;
	edge->c = c;
	edge->val = address_val;

	//printf(" inseriu \n");

	return edge;

}

void remove_path(turn * edge){

	if(edge == NULL){
		return;
	}

	if(edge->adj != NULL){
		free(edge->adj);
	}
	//	printf(" edge %p \n",edge);
	free(edge);	

}


void printf_adj(pto * array){
	int i;
	printf("Adj: ");
	if(array != NULL){
		for(i=0; i<N_NEIGHBOURS; ++i ){
			printf("%p ",array[i]);
		}
		printf("\n");
	}
}


pto path_check(turn ** visited , int k){

	int i;

	for(i = k-1; i > 0; --i){

		//	printf("(%i %i)%i vs %i(%i %i) \n",visited[k]->l,visited[k]->c,ADDR_TO_VAL(visited[k]->val),ADDR_TO_VAL(visited[i]->val),visited[i]->l,visited[i]->c );
		if((visited[k]->l == visited[i]->l) && (visited[k]->c == visited[i]->c)) {
			return (pto)OPT_OK;
		}

	}
	return NULL;
}


pto visited_check(turn ** visited, int k,int id){

	if (visited[k]->adj[id]!= (pto)VISITED){
		//	if(path_check(visited,k) == NULL){
		return NULL;
	}


	return (pto)OPT_OK;
	}

	pto crescent(int val1, int val2, int * val){

		if(val1 < val2){
			*val = val2;				/* next edge need to be bigger than this */
			return (pto)OPT_OK;
		}

		return NULL;
	}

	pto exists_in_path(turn ** path, int size_atual_path, int l, int c){

		int i;

		for(i = size_atual_path; i >= 0; --i){

			//printf(" l: %i c: %i vs %i %i (i = %i)\n",l,c,path[i]->l,path[i]->c,i);
			if((l == path[i]->l) && (c == path[i]->c)) {
				//printf("Ja existe\n");
				return (pto)OPT_OK;
			}

		}
		//printf("Encontrou: %i %i \n",l, c);

		return NULL;

	}




	int dfs(int k, turn ** path, matrix_data * data, pto (*cond)(int val1, int val2, int * val) ){

		int i=0,j=0,id=0,l=0,c=0;
		int val = 0,l0 =0,c0 =0;
		int k_target = data->k;
		int ** matrix_ = data->matrix;

		val = ADDR_TO_VAL(path[k]->val); 
		l0 =(path[k]->l);
		c0 = (path[k]->c);

		for(i=0 ; i<3; ++i){						/* combinations c-1,c, c+1 And l-1,l,l+1 */
			l = ((l0)-1 +i); 				        /* l */

			for(j =0; j<3; ++j){
				c  = ((c0)-1 +j); 		                /* c */
				if(visited_check(path,k,id) == NULL){
					if( move(l,c,l0,c0, data) != NULL){			        /* check is this position is valid !  and not path*/												if( (cond(val,matrix_[l][c],&val) != NULL) /* && (exists_in_path(path,k,l,c)==NULL)*/){  /* check condition for be a edge */
						if((exists_in_path(path,k,l,c)==NULL)){
							path[k]->adj[id]= (pto)VISITED; 	/*add a path tag */	
							k++;					/* found a new edge */	
							path[k] = insert_edge(l,c, ADDR(matrix_[l][c])); /*added the new edge*/
							if(k == k_target){
								return k_target;				/* we found the k length path */
							}
							l0 =(path[k]->l); 
							c0 = (path[k]->c);
							id=0;i=-1;		/* nested loop increments at the end */
							j=0;			/* start again from beginning */
							break;
						}
						else{
							path[k]->adj[id]=(pto)VISITED; 	/* updating adjacency array */
							id++;

						}
					}
					else{
						path[k]->adj[id]=(pto)VISITED; 	/* updating adjacency array */
						id++;
					}
					}
					else{							/* in case of invalid move we assume as a path cell */
						path[k]->adj[id]= (pto)VISITED;
						id++;
					}
				}
				else{
					id++;
				}
			}
		}

		return k; 

	}

	void print_array(turn ** array, int l ){
		int i = 0;
		printf(" array %p %p %p \n",array, array[0],array[i]->val);	
		for(i = l; i >= 0 ; --i){

			printf("(%i) %i |",i,ADDR_TO_VAL(array[i]->val));
		}
		printf(" - turns %i \n",l);
	}

	void free_path(turn ** visited, int k){
		int i =0;
		if(visited!= NULL){
			for(i =0; i<= k; ++i){
				//printf("i= %i  %p\n",i, visited[i]);
				if(visited[i]!= NULL){

					remove_path(visited[i]);
				}
			}
			free(visited);
		}
	}

	/*
	 *	FUNCTION:  	 option_A
	 *	ARGS:		 matrix_data *
	 *	RETURN:		 
	 *	DESCRIPTION: 	 The function that execute the option A. Updates the matrix_data structure  
	 */



	void  option_A(matrix_data * data){

		int ** matrix_ = data->matrix;
		turn ** path = NULL;

		path = (turn **)calloc(data->k+1, sizeof(turn*));
		if(path == NULL){
			printf("*ERROR* Memory allocation failed: DFS() \n");
			exit(0);
		}

		path[0]= insert_edge(data->l0,data->c0, ADDR(matrix_[data->l0][data->c0])); /* starting point */
		data->k_turns = 0; 	


		while(data->k_turns >= 0){

			data->k_turns= dfs(data->k_turns,path, data, crescent);
			// print_array(path, data->k_turns);

			if(data->k_turns == data->k){		/* stop when found a path */
				break;
			}
			if(path[data->k_turns] != NULL){	
				remove_path(path[data->k_turns]);
			}
			data->k_turns--; 
			//printf("Remocao de elmento\n");
			//print_array(path, data->k_turns);
			//printf("DFS -> Return (%i)\n",data->k_turns);

		}


		/* free array vistied e dentro de cada temos que fazer free de adj */
		//printf("*FREE* Vai sair da opcao A - %i\n",data->k_turns);

		data->path = path;

		//free_path(path, data->k_turns);
	}




	void  option_(matrix_data * data){

		int ** matrix_ = data->matrix;
		turn ** path = NULL;

		path = (turn **)calloc(data->k+1, sizeof(turn*));
		if(path == NULL){
			printf("*ERROR* Memory allocation failed: DFS() \n");
			exit(0);
		}

		path[0]= insert_edge(data->l0,data->c0, ADDR(matrix_[data->l0][data->c0])); /* starting point */
		data->k_turns = 0; 	


		while(data->k_turns >= 0){

			data->k_turns= dfs(data->k_turns,path, data, even);
			//  print_array(path, data->k_turns);

			if(data->k_turns == data->k){		/* stop when found a path */
				break;
			}
			if(path[data->k_turns] != NULL){	
				remove_path(path[data->k_turns]);
			}
			data->k_turns--; 
			//printf("Remocao de elmento\n");
			//print_array(path, data->k_turns);
			//printf("DFS -> Return (%i)\n",data->k_turns);

		}

		/* free array vistied e dentro de cada temos que fazer free de adj */
		//printf("*FREE* Vai sair da opcao A - %i\n",data->k_turns);
		data->path = path;
		//free_path(path, data->k_turns);
	}




	void  option_C(matrix_data * data){

		int l=0, c=0;
		int ** matrix_ = data->matrix;
		turn ** path = NULL;

		path = (turn **)calloc(data->k+1, sizeof(turn*));
		if(path == NULL){
			printf("*ERROR* Memory allocation failed: DFS() \n");
			exit(0);
		}

		data->k_turns = 0; 	

		for(l =0; l < data->lines && (data->k_turns != data->k) ; ++l){
			for(c =0; c < data->columns && (data->k_turns != data->k); ++c){

				path[0]= insert_edge(l,c, ADDR(matrix_[l][c])); /* starting point */
				data->k_turns = 0;

				while(data->k_turns >= 0){

					data->k_turns= dfs(data->k_turns,path, data, crescent);
					//print_array(path, data->k_turns);

					if(data->k_turns == data->k){		/* stop when found a path */
						data->l0 = path[0]->l;		/* update the starting point */
						data->c0 = path[0]->c;	
						break;
					}
					if(path[data->k_turns] != NULL){
						remove_path(path[data->k_turns]);
					}
					data->k_turns--;

				}
			}
		}

		/* free array vistied e dentro de cada temos que fazer free de adj */
		//printf("*FREE* Vai sair da opcao A - %i\n",data->k_turns);
		data->path = path;
		//printf("k_turns %i \n", data->k_turns);
		//free_path(path, data->k_turns);
	}




	void  option_D(matrix_data * data){

		int l=0, c=0;
		int ** matrix_ = data->matrix;
		turn ** path = NULL;

		path = (turn **)calloc(data->k+1, sizeof(turn*));
		if(path == NULL){
			printf("*ERROR* Memory allocation failed: DFS() \n");
			exit(0);
		}

		data->k_turns = 0; 	
		//printf("Entrou D: l%i c%i data->k_turns%i \n",l,c,data->k_turns );
		for(l =0; l < data->lines && (data->k_turns != data->k) ; ++l){
			for(c =0; c < data->columns && (data->k_turns != data->k); ++c){

				path[0]= insert_edge(l,c, ADDR(matrix_[l][c])); /* starting point */
				data->k_turns = 0;
				//printf("Entrou loop option_D \n");
				while(data->k_turns >= 0){
					//	printf("starting point %i %i\n",path[0]->l,path[0]->c);
					data->k_turns= dfs(data->k_turns,path, data, even);
					//print_array(path, data->k_turns);

					if(data->k_turns == data->k){		/* stop when found a path */
						data->l0 = path[0]->l;		/* update the starting point */
						data->c0 = path[0]->c;	
						//printf("Solucao option_D \n");

						break;
					}
					if(path[data->k_turns] != NULL){
						remove_path(path[data->k_turns]);
					}
					data->k_turns--;

				}
			}
		}

		/* free array vistied e dentro de cada temos que fazer free de adj */
		//printf("*FREE* Vai sair da opcao A - %i\n",data->k_turns);
		data->path = path;
		//free_path(path, data->k_turns);
	}



	void initalize_path(turn ** master_path, int k){

		int i =0;
		turn * aux =NULL;

		for(i =0; i< k; ++i){

			aux = (turn *)calloc(1, sizeof(turn));

			if(aux == NULL){
				printf("Error mem\n" );
				exit(0);
			}

			aux->l =0;
			aux->l =0;
			aux->adj = NULL; 

			master_path[i] = aux;
		}



	}


	void save_path (turn ** master_path, turn ** found_path, int k){

		turn * aux = NULL; 
		int i;


		if(master_path== NULL ||  found_path == NULL){
			return;
		}




		for(i=0; i<=k ; ++i ){


			if(master_path[i] == NULL){

				aux = (turn *)calloc(1, sizeof(turn));

				if(aux == NULL){
					printf("Error mem\n" );
					exit(0);
				}


				master_path[i] = aux;
			}


			//printf("i:%i path[i]: %p \n",i,master_path[i]);
			memmove(master_path[i], found_path[i], sizeof(turn));
			//master_path[i]->l=  found_path[i]->l; 
			//master_path[i]->c = found_path[i]->c; 
			//master_path[i]->val = found_path[i]->val;

			master_path[i]->adj = NULL;

			//printf("%i - val %i ptr %p\n",i,ADDR_TO_VAL(master_path[i]->val),master_path[i]->val );

		}

	}



	int dfs2(int k, turn ** path, matrix_data * data, pto (*cond)(int val1, int val2, int * val) ){

		int i=0,j=0,id=0,l=0,c=0;
		int val = 0,l0 =0,c0 =0;

		int ** matrix_ = data->matrix;

		val = ADDR_TO_VAL(path[k]->val); 
		l0 =(path[k]->l);
		c0 = (path[k]->c);

		for(i=2 ; i>=0; --i){						/* combinations c-1,c, c+1 And l-1,l,l+1 */
			l = ((l0)-1 +i); 				        /* l */

			for(j =2; j>=0; --j){
				c  = ((c0)-1 +j); 
				//	printf("turn %p (%i %i) - val %i vs saved %i\n", path[k]->adj[id],i,j,matrix_[l][c],val);
				//	printf_adj(path[k]->adj);

				/* c */
				if(visited_check(path,k,id) == NULL){
					if( move(l,c,l0,c0, data) != NULL){	
						//	printf("val  %i vs %i matrix \n",val,matrix_[l][c]);

						/* check is this position is valid !  and not path*/
						if( (cond(val,matrix_[l][c],&val) != NULL) /* && (exists_in_path(path,k,l,c)==NULL)*/){  /* check condition for be a edge */
							if((exists_in_path(path,k,l,c)==NULL)){
								path[k]->adj[id]= (pto)VISITED; 	/*add a path tag */	
								k++;					/* found a new edge */	
								//	printf("insert elemet %i\n",matrix_[l][c]);
								path[k] = insert_edge(l,c, ADDR(matrix_[l][c])); /*added the new edge*/


								l0 =(path[k]->l); 
								c0 = (path[k]->c);
								id=0;i=3;		/* nested loop increments at the end */
								j=2;			/* start again from beginning */
								break;
							}
							else{
								path[k]->adj[id]=(pto)VISITED; 	/* updating adjacency array */
								id++;

							}
						}
						else{
							path[k]->adj[id]=(pto)VISITED; 	/* updating adjacency array */
							id++;
						}
					}
					else{							/* in case of invalid move we assume as a path cell */
						path[k]->adj[id]= (pto)VISITED;
						id++;
					}
				}
				else{
					id++;
				}
			}
		}

		return k; 


	}









	void  option_E(matrix_data * data){

		int l=0, c=0, length = 0;
		int ** matrix_ = data->matrix;
		turn ** path = NULL;
		turn ** max_path = NULL;

		path = (turn **)calloc((data->lines * data->columns)-1, sizeof(turn*)); // fazer alocacao para o maximo de passos
		max_path = (turn **)calloc((data->lines * data->columns)-1, sizeof(turn*)); // fazer alocacao para o maximo de passos

		if(path == NULL){
			printf("*ERROR* Memory allocation failed: DFS() \n");
			exit(0);
		}




		for(l =0; l < data->lines && length < ((data->lines * data->columns) -1) ; ++l){
			for(c =0; c < data->columns && length < ((data->lines * data->columns)); ++c){

				//  printf("cell (%i,%i) \n",l,c);

				path[0]= insert_edge(l,c, ADDR(matrix_[l][c])); /* starting point */
				data->k_turns = 0;  

				while(data->k_turns >= 0){


					data->k_turns= dfs2(data->k_turns,path, data,crescent);
					//print_array(path, data->k_turns);

					if(data->k_turns > length){
						length = data->k_turns;
						data->l0 = path[0]->l;      /* update the starting point */
						data->c0 = path[0]->c;  

						//printf("Encontrou novo caminho %i \n", length);
						save_path(max_path,path,data->k_turns);
						//print_array(path, data->k_turns);
						
						if(length == data->pairs_numbers -1){
							break;	
						}


					}

					if(path[data->k_turns] != NULL){    
						remove_path(path[data->k_turns]);
					}

					data->k_turns--; 

				}


			}
		}
		data->path = max_path;
		//data->k_turns = length;

		/* fazer aqui o free do path*/

		//  printf("*FREE* Vai sair da opcao E %i\n",data->k_turns);
		//print_array(path, data->k_turns);
		free_path(path, data->k_turns);
		//  printf("Fez free DONE - length %i\n",length);
		data->k_turns = length;
	}



	void  option_F(matrix_data * data){

		int l=0, c=0, length = 0;
		int ** matrix_ = data->matrix;
		turn ** path = NULL;
		turn ** max_path = NULL;

		path = (turn **)calloc((data->lines * data->columns), sizeof(turn*)); // fazer alocacao para o maximo de passos
		max_path = (turn **)calloc((data->lines * data->columns), sizeof(turn*)); // fazer alocacao para o maximo de passos

		if(path == NULL){
			printf("*ERROR* Memory allocation failed: DFS() \n");
			exit(0);
		}




		//printf("Entrou option_F \n");
		for(l =0; l < data->lines && (length < data->pairs_numbers ) && length <((data->lines * data->columns) - 1); ++l){
			for(c =0; c < data->columns &&  (length < data->pairs_numbers) && length < ((data->lines * data->columns) - 1); ++c){

				//	printf("cell (%i,%i) \n",l,c);

				path[0]= insert_edge(l,c, ADDR(matrix_[l][c])); /* starting point */
				data->k_turns = 0; 	
				//printf("atribui path[0]\n ");
				while(data->k_turns >= 0){


					data->k_turns= dfs2(data->k_turns,path, data,even);
					//print_array(path, data->k_turns);

					if(data->k_turns > length){
						
						length = data->k_turns;
						data->l0 = path[0]->l;		/* update the starting point */
						data->c0 = path[0]->c;	

						//printf(" %i %i Encontrou novo caminho %i - %i \n", path[0]->l,path[0]->c,length, ADDR_TO_VAL(path[0]->val));
						save_path(max_path,path,data->k_turns);

						if(length == data->pairs_numbers -1){
							//printf("Maximo\n");
							//print_array(max_path, length);
							data->path = max_path;
							//free_path(path, data->k_turns);
		//printf("Fez free DONE - length %i\n",length);
							data->k_turns = length;
							
							return;	
						}
					}

					if(path[data->k_turns] != NULL){	
						//printf("Removing elment\n");
						remove_path(path[data->k_turns]);
						//printf("DONE removing \n");
					}

					data->k_turns--; 

				}


			}
		}
		//print_array(path, length);
		//printf("DONE path \n");
		data->path = max_path;
		//data->k_turns = length;

		/* fazer aqui o free do path*/

		//printf("*FREE* Vai sair da opcao F %i\n",data->k_turns);
		//print_array(path, data->k_turns);
		free_path(path, data->k_turns);
		//printf("Fez free DONE - length %i\n",length);
		data->k_turns = length;

		//printf("vai sair da funcao F\n");
}



















	/*
	 *	FUNCTION:  	 write_file
	 *	ARGS:		 FILE * , char  * , matrix_data *
	 *	RETURN:		 FILE *
	 *	DESCRIPTION:  	 Write the information inside of matrix_data to a file	
	 */

	void  write_file(FILE *ptr, char * namefile, matrix_data * data){
		//printf("Wrinting.. \n ");
		if (fprintf(ptr, "%d %d %c %d %d %d %d \n",data->lines, data->columns,data->option,data->l0,data->c0,data->k,data->k_turns) <0){
			printf("*ERROR* Writing to file %s failed\n", namefile); 
			exit(0);
		}
		//printf("k_turns %i \n", data->k_turns);
		return;
	}


	/*
	 *	FUNCTION:  	 write_path
	 *	ARGS:		 FILE * , matrix_data *
	 *	RETURN:		 FILE *
	 *	DESCRIPTION:  	 Write the information inside of matrix_data to a file	
	 */

	void  write_path(FILE *ptr ,matrix_data * data){

		int i =0;
		//printf("Writing path %p\n",data->path);
		//print_array(data->path, data->k_turns);


		if(data->k_turns != -1){
			for(i=1; i<=data->k_turns; ++i){ /*na opcao C tens de ser <= */
				//printf("write %i times ptr %p \n",i,data->path[i]);
				if (fprintf(ptr, "%d %d %d \n",data->path[i]->l, data->path[i]->c,ADDR_TO_VAL(data->path[i]->val)) <0){
					printf("*ERROR* Writing to file failed\n"); 
					exit(0);
				}
			}	
		}
		//printf("Writing DONE\n ");
		fprintf(ptr,"\n");
		return;
	}

	/*
	 *	FUNCTION:  	 fill_matrix
	 *	ARGS:		 FILE * , matrix_data *
	 *	RETURN:		 
	 *	DESCRIPTION:  	 Allocate memory for the matrix data
	 */


	void  fill_matrix (FILE * ptr, matrix_data * data){

		int i =0, j=0, fl =0;
		int ** matrix_ = NULL;	

		data->pairs_numbers =0;
		matrix_ = data->matrix;

		if(matrix_ == NULL){ 
			printf("*ERROR* Need memory allocation: fill_matrix() \n");
			exit(0); }

		for (i = 0; i <data->lines; i++) {
			matrix_[i] = (int *)calloc((data->columns),sizeof(int));
			for (j = 0; (j < data->columns) && fl >-1; j++) {
				fl =fscanf(ptr, "%d", &matrix_[i][j]);
				
				if((matrix_[i][j])%2 ==0){
					(data->pairs_numbers)++; 
				//		printf(" (%i) ",matrix_[i][j]);
				}
			//	else{printf(" %i ",matrix_[i][j]);}
			}
			//	printf("\n");
		}

	//	printf("Readed: %i pair numbers \n",(data->pairs_numbers));
	}



	/*
	 *	FUNCTION:  	 fill_matrix
	 *	ARGS:		 FILE * , matrix_data *
	 *	RETURN:		 
	 *	DESCRIPTION:  	 Allocate memory for the matrix data
	 */


	void  fill_matrix_smart (FILE * ptr, matrix_data * data){

		int i =0, j=0, fl =0;
		int bucket = 0;
		int ** matrix_ = NULL;	

		matrix_ = data->matrix;

		if(matrix_ == NULL){ 
			printf("*ERROR* Need memory allocation: fill_matrix() \n");
			exit(0); }

		for (i = 0; i <data->lines; i++) {
				
			//printf("line %i vs %i \n",i,data->l0 - (2*data->k) );
				if( (i > (data->l0 - (2*data->k))) && (i <  (data->l0 + (2*data->k))) ){
					matrix_[i] = (int *)calloc((data->columns),sizeof(int));}
				else{
					matrix_[i] = NULL;
				}
			
			for (j = 0; (j < data->columns) && fl >-1; j++) {
				
				if( (i > (data->l0 - (2*data->k))) && (i <  (data->l0 + (2*data->k))) ){
					fl =fscanf(ptr, "%d", &matrix_[i][j]);
				}
				else{
					fl =fscanf(ptr, "%d", &bucket);
				}
							//	else{printf(" %i ",matrix_[i][j]);}
			}
			//	printf("\n");
		}

	//	printf("Readed: %i pair numbers \n",(data->pairs_numbers));
	//printf("DONE \n");
	}


	pto crescente(int val1, int val2){

		if(val1<val2){
			return NULL;
		}

		return (pto)OPT_OK;
	}




	void allocation_matrix(matrix_data * data){

		if(data->lines < 1 || data->columns < 1){
			printf("*ERROR* File is corrupted: allocation_matrix() \n");
			exit(0);
		}
		data->matrix = (int **)malloc((data->lines) *sizeof(int *));
		if(data->matrix == NULL){
			printf("*ERROR* Memory allocation for matrix failed: allocation_matrix() \n" );
			exit(0);
		}
	}






	/*
	 *	FUNCTION:  	 option_validation
	 *	ARGS:		 char
	 *	RETURN:		 (void *)
	 *	DESCRIPTION: 	 check if option (character) is valid. Invalid case returns NULL, otherwise returns OPT_OK
	 */


	pto option_validation( matrix_data * data, FILE * ptr){

		switch (data->option) {

			case 'A': 
				/*	printf("A\n"); */
				if (intial_coordinates(data) == NULL || k_validation(data) == NULL){
					return NULL;	}
				allocation_matrix(data);
				fill_matrix_smart(ptr,data);	
				option_A(data);
				break; 
			case 'B': 
				/*	printf("B\n"); */
				if (intial_coordinates(data) == NULL || k_validation(data) == NULL){
					return NULL;	}
				allocation_matrix(data);
				fill_matrix_smart(ptr,data);	
				option_(data);
				break;
			case 'C': 
				//printf("C\n");
				if (k_validation(data) == NULL){
					return NULL;	
				}
				allocation_matrix(data);
				fill_matrix(ptr,data);	
				option_C(data);
				break; 
			case 'D':
				if (k_validation(data) == NULL){
					return NULL;
				}
				allocation_matrix(data);
				//printf("Op D filled\n");
				fill_matrix(ptr,data);
				option_D(data);
				//printf("Op D DONE\n ");
				break;
			case 'E':
				if (k_validation(data) == NULL){
					return NULL;
				}
				allocation_matrix(data);
				//printf("Op D filled\n");
				fill_matrix(ptr,data);
				option_E(data);
				//printf("Op D DONE\n ");
				break;
			case 'F':
				if (k_validation(data) == NULL){
					return NULL;
				}
				allocation_matrix(data);
				//printf("Op D filled\n");
				fill_matrix(ptr,data);
				option_F(data);


				//printf("Op F DONE: %i %i %i %i \n", data->l0,data->c0,data->k,data->k_turns);
				break;



			default: 
				/* Invalid option */ 
				return NULL;
		}

		return (void *)OPT_OK;

	}





	/*
	 *	FUNCTION:  	 run_file
	 *	ARGS:		 FILE * , char  * , matrix_data *	
	 *	RETURN:		 FILE *
	 *	DESCRIPTION: 	 Fill the structure with data read from input file and executes the options. If invalid because somehow, k =-1 and skip the data storage process
	 */ 

	void  run_file(FILE * ptr, char * namefile , matrix_data * data ,FILE * ptw, char * name_out){
		/* use a pointer to function for respective option_func  - pass as argument to option validation */ 

		int return_val = 0;
		int debug =1;
		data->matrix = NULL; 
		if ( ptr == NULL ) {
			fprintf ( stderr, "*ERROR* cannot open file '%s'\n", namefile);
			exit (0);
		}

		/* check if the option character is valid */
		while ( ((return_val = fscanf(ptr, "%d %d %c %d %d %d \n",&(data->lines), &(data->columns), &(data->option), &(data->l0), &(data->c0),&(data->k) )) != EOF ) ){
			//printf("Reading.. (%i), %i %i %c %i %i %i \n",debug,data->lines,data->columns,(data->option), (data->l0), (data->c0),(data->k) );

			if(return_val == -1 || return_val < 1 ){
				printf("*ERROR* File %s corrupted: run_file() \n",namefile);
				break;
			}

			if(return_val == N_VARS && option_validation(data,ptr) != NULL){
				write_file(ptw,name_out,data);
				//printf(" escreveu cabecalho\n");
				write_path(ptw,data);
				//printf("escreveu caminho\n");
				free_path(data->path, data->k_turns);
				//printf("free data->path\n");
				free_matrix(data);
				//printf(" free data\n");


			}
			else if (return_val == N_VARS && option_validation(data,ptr) == NULL) { 
				data->k_turns = INVALID;  
				write_file(ptw,name_out,data);
				write_path(ptw,data);
				/* update read pointer */ 
				skip_parsing(ptr,(data->lines * data->columns));
				find_next_blank_line(ptr); 
			}
			else{ }

			++debug;

		}



	}






