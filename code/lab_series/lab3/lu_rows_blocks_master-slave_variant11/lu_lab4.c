#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

struct my_vector
{
	int size;
	double data[1];
};
struct my_matrix
{
	int rows;
	int cols;
	double data[1];
};

struct my_vector *vector_alloc(int size, double initial);
struct my_matrix *read_matrix(const char *filename);
struct my_matrix *matrix_alloc(int rows, int cols, double initial);
void matrix_print(struct my_matrix *mat);
void fatal_error(const char *message, int errorcode);

const char *input_file_MA = "MA.txt";
const int COLUMN_TAG = 0x1;
int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int np, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    struct my_matrix *MA;
    int N;
    if(rank == 0)
    {
        MA = read_matrix(input_file_MA);
        if(MA->rows != MA->cols) {
            fatal_error("Matrix is not square!", 4);
        }
        N = MA->rows;
				matrix_print(MA);
				printf("\n");
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int part = N / np;
    struct my_matrix *MAh = matrix_alloc(N, part, .0);
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

    struct my_vector *current_l = vector_alloc(N, .0);
    struct my_matrix *MLh = matrix_alloc(N, part, .0);
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
    	   /* Модифікація стовпців матриці МА відповідно до поточного l_i */
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

struct my_vector *vector_alloc(int size, double initial)
{
  	struct my_vector *result = (struct  my_vector *) malloc(sizeof(struct my_vector) + (size - 1) * sizeof(double));
  	result->size = size;

  	for (int i = 0; i < size; i++)
  	{
  		result->data[i] = initial;
  	}
  	return result;
}
struct my_matrix *read_matrix(const char *filename)
{
  	FILE *mat_file = fopen(filename, "r");
  	if (mat_file == NULL)
  	{
  		fatal_error("can't open matrix file", 1);
  	}
  	int rows;
  	int cols;
  	fscanf(mat_file, "%d %d", &rows, &cols);

  	struct my_matrix *result = matrix_alloc(rows, cols, 0.0);
  	for (int i = 0; i < rows; i++)
  	{
  		for (int j = 0; j < cols; j++)
  		{
  			fscanf(mat_file, "%lf", &result->data[i * cols + j]);
  		}
  	}
  	fclose(mat_file);
  	return result;
}
struct my_matrix *matrix_alloc(int rows, int cols, double initial)
{
  	struct my_matrix *result = (struct  my_matrix *) malloc(sizeof(struct my_matrix) + (rows * cols - 1) * sizeof(double));
  	result->rows = rows;
  	result->cols = cols;

  	for (int i = 0; i < rows; i++)
  	{
  		for (int j = 0; j < cols; j++)
  		{
  			result->data[i * cols + j] = initial;
  		}
  	}
  	return result;
}
void matrix_print(struct my_matrix *mat)
{
  	for (int i = 0; i < mat->rows; i++)
  	{
  		for (int j = 0; j < mat->cols; j++)
  		{
  			printf("%5.2lf ", mat->data[i * mat->cols + j]);
  		}
  		printf("\n");
  	}
}
void fatal_error(const char *message, int errorcode)
{
  	printf("fatal error: code %d, %s\n", errorcode, message);
  	fflush(stdout);
  	MPI_Abort(MPI_COMM_WORLD, errorcode);
}
