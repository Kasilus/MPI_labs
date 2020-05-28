#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "matrix_vector_utils.h"

void fatal_error(const char *message, int errorcode);

const char *input_file_MA = "MA.txt";
const int COLUMN_TAG = 0x1;
int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int np, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    struct Matrix *MA;
    int N;
    if(rank == 0)
    {
        MA = read_matrix(input_file_MA);
        if(MA->rows != MA->cols) {
            fatal_error("Matrix is not square!", 4);
        }
        N = MA->rows;
				print_matrix(MA);
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int part = N / np;
    struct Matrix *MAh = matrix_alloc(N, part, .0);
    MPI_Datatype matrix_columns;
    MPI_Type_vector(N*part, 1, np, MPI_DOUBLE, &matrix_columns);
    MPI_Type_commit(&matrix_columns);
    MPI_Datatype vector_struct;
    MPI_Aint extent;
    MPI_Type_extent(MPI_INT, &extent);
    MPI_Aint offsets[] = {0, extent};
    int lengths[] = {1, N+1};
    MPI_Datatype oldtypes[] = {MPI_INT, MPI_DOUBLE};
    MPI_Type_struct(2, lengths, offsets, oldtypes, &vector_struct);
    MPI_Type_commit(&vector_struct);

    if(rank == 0)
    {
      	for(int i = 1; i < np; i++)
      	{
            MPI_Send(&(MA->data[i]), 1, matrix_columns, i, COLUMN_TAG, MPI_COMM_WORLD);
      	}
    	  for(int i = 0; i < part; i++)
       	{
        int col_index = i*np;
        	    for(int j = 0; j < N; j++)
         	    {
                  MAh->data[j*part + i] = MA->data[j*N + col_index];
          	  }
        }
      free(MA);
    }
    else
    {
        MPI_Recv(MAh->data, N*part, MPI_DOUBLE, 0, COLUMN_TAG,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    struct Vector *current_l = vector_alloc(N, .0);
    struct Matrix *MLh = matrix_alloc(N, part, .0);
    for(int step = 0; step < N-1; step++)
    {
         if (step % np == rank)
    	   {
            int col_index = (step - (step % np)) / np;
            MLh->data[step*part + col_index] = 1.;
            for(int i = step+1; i < N; i++)
            {
                MLh->data[i*part + col_index] = MAh->data[i*part + col_index] /
        MAh->data[step*part + col_index];
            }
            for(int i = 0; i < N; i++)
            {
                current_l->data[i] = MLh->data[i*part + col_index];
            }
    	   }
    	   MPI_Bcast(current_l, 1, vector_struct, step % np, MPI_COMM_WORLD);
         for(int i = step+1; i < N; i++)
    	   {
             for(int j = 0; j < part; j++)
             {
                 MAh->data[i*part + j] -= MAh->data[step*part + j] * current_l->data[i];
             }
    	   }
    }
    double prod = 1.;
    for(int i = 0; i < part; i++)
    {
        int row_index = i*np + rank;
    	  prod *= MAh->data[row_index*part + i];
    }
		MPI_Allreduce(MPI_IN_PLACE, &prod, 1, MPI_DOUBLE, MPI_PROD, MPI_COMM_WORLD);
    if(rank == 0)
    {
			printf("DET = %f\n", prod);
    }
    MPI_Type_free(&matrix_columns);
    MPI_Type_free(&vector_struct);
    return MPI_Finalize();
}

void fatal_error(const char *message, int errorcode)
{
  	printf("fatal error: code %d, %s\n", errorcode, message);
  	fflush(stdout);
  	MPI_Abort(MPI_COMM_WORLD, errorcode);
}
