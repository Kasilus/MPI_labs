#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

struct my_vector *read_vector(const char *filename);
struct my_vector *vector_alloc(int size, double initial);
struct my_matrix *read_matrix(const char *filename);
struct my_matrix *matrix_alloc(int rows, int cols, double initial);
void vector_print(struct my_vector *vect);
void matrix_print(struct my_matrix *mat);
void fatal_error(const char *message, int errorcode);

int main(int argc, char *argv[])
{
    const char *input_file_MA = "MA_2.txt";
    const char *output_file_x = "x.txt";
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
    }

		MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int part = N / np;
    struct my_matrix *MAh = matrix_alloc(part, N, .0);
    if(rank == 0)
    {
        MPI_Scatter(MA->data, N*part, MPI_DOUBLE, MAh->data, N*part, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        free(MA);
    }
    else
    {
        MPI_Scatter(NULL, 0, MPI_DATATYPE_NULL, MAh->data, N*part, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    struct my_vector *current_MA = vector_alloc(N, .0);
    struct my_matrix *MLh = matrix_alloc(part, N, .0);
    for(int step = 0; step < N-1; step++)
    {

				int row;
				if (rank == 0) {
						row = step;
				} else {
						row = step % rank;
				}
				for(int i = 0; i < N; i++)
				{
						current_MA->data[i] = MAh->data[row * N + i];
				}

				 MPI_Request send_req;
				 if (step / part == rank && rank != 0) {
				     MPI_Isend(current_MA, N+1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &send_req);
				     MPI_Wait(&send_req, MPI_STATUS_IGNORE);
				 }
				 MPI_Request recv_req;
				 if (rank == 0) {
				    if (step / part != 0) {
				        MPI_Irecv(current_MA, N+1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &recv_req);
								MPI_Wait(&recv_req, MPI_STATUS_IGNORE);
				    }
				 }
				 MPI_Bcast(current_MA, N+1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

				if (step < (rank+1) * part - 1) {
					int stopI = step + 1;
					if ((((rank+1) * part) / stopI) > 0) {
							stopI += part;
					} else {
							stopI += stopI % ((rank+1) * part);
					}
					int col_index = step;
					for(int i = step+1; i < stopI; i++)
					{
							MLh->data[(i % part) * N + col_index] = MAh->data[(i % part) * N + col_index] /
			current_MA->data[step];
						for(int j = step+1; j < N; j++)
						{
								MAh->data[(i % part) * N + j] -= current_MA->data[j] * MLh->data[(i % part) * N + step];
						}
					}
				}
    }

    double prod = 1.;
    for(int i = 0; i < part; i++)
    {
        int row_index = i*np + rank;
    	  prod *= MAh->data[row_index*part + i];
    }

    if(rank == 0)
    {
        MPI_Reduce(MPI_IN_PLACE, &prod, 1, MPI_DOUBLE, MPI_PROD, 0, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Reduce(&prod, NULL, 1, MPI_DOUBLE, MPI_PROD, 0, MPI_COMM_WORLD);
    }

    if(rank == 0)
    {
        printf("DET = %f\n", prod);
    }
    free(MAh);
		free(current_MA);
		free(MLh);
    return MPI_Finalize();
}

struct my_vector *read_vector(const char *filename)
{
    FILE *vect_file = fopen(filename, "r");
    if (vect_file == NULL)
    {
      fatal_error("can't open vector file", 1);
    }
    int size;
    fscanf(vect_file, "%d", &size);

    struct my_vector *result = vector_alloc(size, 0.0);
    for (int i = 0; i < size; i++)
    {
        fscanf(vect_file, "%lf", &result->data[i]);
    }
    fclose(vect_file);
    return result;
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
void vector_print(struct my_vector *vect)
{
  	for (int i = 0; i < vect->size; i++)
  	{
  			printf("%5.2lf ", vect->data[i]);
    }
  	printf("\n");
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
