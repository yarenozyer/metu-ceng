#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "hw2_output.h"



typedef struct ThreadArgsForAddition{
    int matrix_id;
    unsigned row_id;
    unsigned col_id;
    int common_dim; //common dimension between result_1 and result_2
    int row_size;//how many row
    int col_size;//how many col
    int* first_row;
    int* second_row; //will be used as column in multiplication
    int** matrix; //will be used for column calc
    int* result_row;
    sem_t* row_sems;
    sem_t** sems;
}thread_args;





void print_matrix(int row, int col, int** matrix){
    for(int i=0; i<row;i++){
        for(int j=0; j< col; j++){
            if(j == col-1) printf("%d", matrix[i][j]);
            else printf("%d ", matrix[i][j]);
        }
        if(i != row-1)printf("\n");
    }
    
    return;
}

void free_matrix(int row, int col, int** matrix){
    for(int i=0; i<row;i++){

        free(matrix[i]);
    }
    free(matrix);
    return;
}

void *thread_add(void *arg){ // thread function for addition
    thread_args* args = (thread_args*) arg; //cast the thread arguments back to struct
    for (int j = 0; j < args->col_size; j++)
    {
        args->result_row[j] = args->first_row[j] + args->second_row[j];
        hw2_write_output(args->matrix_id, args->row_id +1 ,(unsigned) j+1, args->result_row[j]); 
        if(args->matrix_id==1) sem_post(&args->sems[args->row_id][j]);

    }
    if(args->matrix_id ==0) sem_post(&args->row_sems[args->row_id]);
    return NULL;
}

void *thread_multiply(void *arg){ // thread function for multiplication
    thread_args* args = (thread_args*) arg; // cast the thread arguments back to struct
    int temp=0;

    
    
    sem_wait(&args->row_sems[args->row_id]); //wait for row semaphores
    
    
    for(int i=0;i< args->col_size; i++){ //col_size=col_2
        for(int k=0;k< args->common_dim;k++){
            sem_wait(&args->sems[k][i]);//wait for col semaphores
        }
        temp=0;
        for (int j = 0; j < args->common_dim; j++) //row_size = row_1 common_dim = row_2
        {
            temp += args->first_row[j] * args->matrix[j][i];
        }

        hw2_write_output(args->matrix_id, args->row_id+1,(unsigned) i+1, temp);
        args->result_row[i] = temp;
        for(int k=0;k< args->common_dim;k++){
            sem_post(&args->sems[k][i]);// signal the col semaphores
        }
        


        
    }
    sem_post(&args->row_sems[args->row_id]);
    return NULL;
}

int main(){
    hw2_init_output(); //function for calculating time

    /******* INITIALIZE ALL MATRICES *******/
    int row_1, col_1, row_2, col_2;

    scanf("%d %d", &row_1, &col_1); //get the dimensions of the first matrix

    int** matrix_1 = malloc(sizeof(int*)*row_1);

    for(int i=0; i<row_1;i++){
        matrix_1[i] = malloc(sizeof(int)*col_1);
        for(int j=0; j< col_1; j++){
             scanf("%d", &matrix_1[i][j]);
        }
           
    }
    scanf("%d %d", &row_1, &col_1); //get the dimensions of the first matrix

    int** matrix_2 = malloc(sizeof(int*)*row_1);

    for(int i=0; i<row_1;i++){
        matrix_2[i] = malloc(sizeof(int)*col_1);
        for(int j=0; j< col_1; j++){
             scanf("%d", &matrix_2[i][j]);
        }
           
    }

    scanf("%d %d", &row_2, &col_2);
    
    int** matrix_3 = malloc(sizeof(int*)*row_2);

    for(int i=0; i<row_2;i++){
        matrix_3[i] = malloc(sizeof(int)*col_2);
        for(int j=0; j< col_2; j++){
             scanf("%d", &matrix_3[i][j]);
        }
           
    }
    
    scanf("%d %d", &row_2, &col_2);

    int** matrix_4 = malloc(sizeof(int*)*row_2);

    for(int i=0; i<row_2;i++){
        matrix_4[i]= malloc(sizeof(int)*col_2);
        for(int j=0; j< col_2; j++){
             scanf("%d", &matrix_4[i][j]);
        }
           
    }
    
    int** add_1 = malloc(sizeof(int*)*row_1); //result matrix of the first addition
    for(int i=0; i<row_1;i++){
        add_1[i]= malloc(sizeof(int)*col_1); 
    }

    int** add_2 = malloc(sizeof(int*)*row_2); //result matrix of the second addition
    for(int i=0; i<row_2;i++){
        add_2[i]= malloc(sizeof(int)*col_2); 
    }

    int** final = malloc(sizeof(int*)*row_1); //result matrix of the multiplication
    for(int i=0; i<row_1;i++){
        final[i]= malloc(sizeof(int)*col_2); 
    }

     /******* end of initializion of matrices *******/

    int thread_num=row_1 + row_2 + row_1; // N + M + N number of threads
    pthread_t threads[thread_num];
    int thread_ids[thread_num];
    thread_args* args = malloc(sizeof(thread_args)*thread_num);
    
    int** columns;
    columns = malloc(sizeof(int*)* col_2);
    int col_index = 0;

     //row and column should be ready for calc
    sem_t** sems;
    sems = malloc(sizeof(sem_t*) * row_2);
    sem_t* row_sems = malloc(sizeof(sem_t) * row_1);

    for(int i=0;i< row_1; i++) sem_init(&row_sems[i], 0, 0);
    for(int i=0;i<row_2; i++){
        sems[i] = malloc(sizeof(sem_t) * col_2);
        for(int j =0; j< col_2; j++){
            sem_init(&sems[i][j], 0, 0); // initialize semaphores for rows
        }
    } 
    

    for(int i=0;i< thread_num; i++){
        thread_ids[i]=i;
        if(i<row_1){ //first addition threads
            args[i].matrix_id = 0;
            args[i].row_id = i;
            args[i].row_size = row_1;
            args[i].col_size= col_1;
            args[i].first_row = matrix_1[i];
            args[i].second_row = matrix_2[i];
            args[i].result_row = add_1[i];
            args[i].row_sems = row_sems;
            pthread_create(&threads[i], NULL, thread_add, (void *)&args[i]); 
        }
        else if(i>=row_1 && i< row_1 + row_2){ //second addition threads
            args[i].matrix_id = 1;
            args[i].row_id = i-row_1;
            args[i].row_size = row_2;
            args[i].col_size= col_2;
            args[i].first_row = matrix_3[i-row_1];
            args[i].second_row = matrix_4[i-row_1];
            args[i].result_row = add_2[i-row_1];
            args[i].sems = sems;
            pthread_create(&threads[i], NULL, thread_add, (void *)&args[i]); 
        }
        
        else if(i>= row_1 + row_2){ //multiplication threads
            args[i].matrix_id = 2;
            args[i].row_id = i-row_1-row_2;
            args[i].col_id = col_index;
            args[i].common_dim = col_1; // = row_2
            args[i].row_size = row_1;
            args[i].col_size= col_2;
            args[i].first_row = add_1[i-row_1-row_2];
            args[i].second_row = NULL;
            args[i].result_row = final[i-row_1-row_2];
            args[i].matrix = add_2;
            args[i].row_sems = row_sems;
            args[i].sems = sems;
            col_index++;
            pthread_create(&threads[i], NULL, thread_multiply, (void *)&args[i]);
        } 
    }

    
    for(int i=0;i< thread_num; i++){
        
        pthread_join(threads[i], NULL);
    }


    //dont forget to destroy semaphores

    print_matrix(row_1, col_2, final); //print the final matrix

    //free all the matrices
    free_matrix(row_1, col_1, matrix_1);
    free_matrix(row_1, col_1, matrix_2);
    free_matrix(row_2, col_2, matrix_3);
    free_matrix(row_2, col_2, matrix_4);
    free_matrix(row_1, col_1, add_1);
    free_matrix(row_2, col_2, add_2);
    free_matrix(row_1, col_2, final);
    return 0;

}
