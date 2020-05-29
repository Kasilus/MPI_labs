#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix_vector_utils.h"

void fatal_error(const char *message, int errorcode);

int main(int argc, char *argv[])
{
    const char *input_file_MA = "MA.txt";
    const char *output_file_x = "det.out";
    MPI_Init(&argc, &argv);
    int np, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double start, end;
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    struct Matrix *MA;
    int N;
    if(rank == 0)
    {
        MA = read_matrix(input_file_MA);
        if(MA->rows != MA->cols) {
            fatal_error("Matrix is not square!", 4);
        }
        N = MA->rows;
				// print_matrix(MA);
    }

		MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int part = N / np;
    struct Matrix *MAh = matrix_alloc(part, N, .0);
    if(rank == 0)
    {
        MPI_Scatter(MA->data, N*part, MPI_DOUBLE, MAh->data, N*part, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        free(MA);
    }
    else
    {
        MPI_Scatter(NULL, 0, MPI_DATATYPE_NULL, MAh->data, N*part, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    struct Vector *current_MA = vector_alloc(N, .0);
    struct Matrix *MLh = matrix_alloc(part, N, .0);
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
        // printf("DET = %f\n", prod);
    }
    free(MAh);
		free(current_MA);
		free(MLh);

    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();

    MPI_Finalize();
    if (rank == 0) {
        printf("Runtime = %f\n", (end-start) / total_runs);
    }
    return 0;
}

void fatal_error(const char *message, int errorcode)
{
  	printf("fatal error: code %d, %s\n", errorcode, message);
  	fflush(stdout);
  	MPI_Abort(MPI_COMM_WORLD, errorcode);
}
