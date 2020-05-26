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

/* Основна функція */
int main(int argc, char *argv[])
{
		const int LOGGED_RANK = 1;
    const char *input_file_MA = "MA_2.txt";
    const char *output_file_x = "x.txt";
    /* Ініціалізація MPI */
    MPI_Init(&argc, &argv);
    /* Отримання загальної кількості задач та рангу поточної задачі */
    int np, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    /* Зчитування даних в задачі 0 */
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

		/* Розсилка всім задачам розмірності матриць та векторів */
		/* int MPI_Bcast(void *buffer, int count, MPI_Datatype, int root, MPI_Comm comm);
		 * [IN/OUT] buffer - покажчик буферу відправки (для задачі root), для інших -
		 * покажчик буферу прийому.
		 * count - кілкість елементів у буфері = 1
		 * root - ранг задачі, що відправляє дані = 0
		 *  */
		MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /* Обчислення частини матриці, яка буде зберігатися в кожній
    * задачі, вважаемо що N = k*np. Виділення пам’яті для зберігання частин
    * матриць в кожній задачі та встановлення їх початкових значень */
    int part = N / np;
    struct my_matrix *MAh = matrix_alloc(part, N, .0);
    /* Розбиття вихідної матриці MA на частини по part рядків та розсилка частин
    * у всі задачі. Рядки мають йти один за одним, щоб у кожній задачі опинився
		* послідовний блок рядків. Звільнення пам’яті, виділеної для матриці МА. */
    if(rank == 0)
    {
			  /* MPI_Scatter
         * Розсилає всім задачам окремі незалежні частини однакового розміру вихідного буфера.
				 * MPI_Scatter(void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm);
				 */
        MPI_Scatter(MA->data, N*part, MPI_DOUBLE, MAh->data, N*part, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        free(MA);
    }
    else
    {
        MPI_Scatter(NULL, 0, MPI_DATATYPE_NULL, MAh->data, N*part, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

		/* Поточне значення вектору l_i */
    struct my_vector *current_MA = vector_alloc(N, .0);
    /* Частина стовпців матриці L */
    struct my_matrix *MLh = matrix_alloc(part, N, .0);
    /* Основний цикл ітерації (кроки) */
    for(int step = 0; step < N-1; step++)
    {
				if (rank == LOGGED_RANK) {
			  		printf("STEP:%d\n", step);
				}
				/* Формування вектору MA на відправку іншим процесорам */
				for(int i = 0; i < N; i++)
				{
						int row;
						if (rank == 0) {
								row = step;
						} else {
								row = step % (rank+1);
						}
						current_MA->data[i] = MAh->data[row * N + i];
				}

				if (rank == LOGGED_RANK) {
						printf("BEFORE BCAST\n");
						printf("MAh\n");
						matrix_print(MAh);
						printf("Current_MA\n");
						vector_print(current_MA);
				}
				/* Розсилка поточних значень l_i */
				/* int MPI_Bcast(void *buffer, int count, MPI_Datatype, int root, MPI_Comm comm);
				 * [IN/OUT] buffer - покажчик буферу відправки (для задачі root), для інших -
				 * покажчик буферу прийому.
				 * count - кілкість елементів у буфері
				 * root - ранг задачі, що відправляє дані
				 *  */
				 MPI_Bcast(current_MA, N+1, MPI_DOUBLE, step / np, MPI_COMM_WORLD);
				 if (rank == LOGGED_RANK) {
 						printf("AFTER BCAST\n");
 						printf("MAh\n");
 						matrix_print(MAh);
 						printf("Current_MA\n");
 						vector_print(current_MA);
 				}

				if (rank == LOGGED_RANK) {
						printf("MLh BEFORE\n");
						matrix_print(MLh);
				}

				if (step < (rank+1) * part - 1) {
					int stopI = (step + 1);
					if ((step+1) == (rank+1) * part - 1) {
							stopI++;
					} else {
							stopI += part;
					}
					int col_index = step;
					if (rank == LOGGED_RANK) {
							printf("STOP_I = %d\n", stopI);
							printf("COL_INDEX = %d\n", col_index);
					}
					for(int i = step+1; i < stopI; i++)
					{
							if (rank == LOGGED_RANK) {
									printf("step,i = %d,%d\n", step, i);
									printf("Mlh[%d]\n", (i % part) * N + col_index);
									printf("MAh->data[%d] = %f\n", (i % part) * N + col_index, MAh->data[(i % part) * N + col_index]);
									printf("current_MA->data[%d] = %f\n", (i % part) * N, current_MA->data[step]);
							}
							MLh->data[(i % part) * N + col_index] = MAh->data[(i % part) * N + col_index] /
			current_MA->data[step];
							if (rank == LOGGED_RANK) {
									printf("NEW Mlh[%d] = %f\n", (i % part) * N, MLh->data[(i % part) * N]);
							}
						for(int j = step+1; j < N; j++)
						{
								if (rank == LOGGED_RANK) {
										printf("step,i,j = %d,%d,%d\n", step, i, j);
										printf("MAh->data[%d] = %f\n", (i % part) * N + j, MAh->data[(i % part) * N + j]);
										printf("current_MA->data[%d] = %f\n", j, current_MA->data[j]);
										printf("MLh->data[%d] = %f\n", (i % part) * N + step, MLh->data[(i % part) * N + step]);
								}
								MAh->data[(i % part) * N + j] -= current_MA->data[j] * MLh->data[(i % part) * N + step];
								if (rank == LOGGED_RANK) {
										printf("NEW MAh->data[%d] = %f\n", (i % part) * N + j, MAh->data[(i % part) * N + j]);
								}
						}
					}
					if (rank == LOGGED_RANK) {
							printf("MLh AFTER\n");
							matrix_print(MLh);
							printf("MAh AFTER\n");
							matrix_print(MAh);
					}
				}
    }

    /* Обислення добутку елементів, які знаходяться на головній діагоналі
    * основної матриці (з урахуванням номеру стовпця в задачі) */
    double prod = 1.;
    for(int i = 0; i < part; i++)
    {
        int row_index = i*np + rank;
    	  prod *= MAh->data[row_index*part + i];
    }

		/* Згортка добутків елементів головної діагоналі та вивід результату в задачі 0 */
    if(rank == 0)
    {
				/* int MPI_Reduce(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
MPI_Op op, int root, MPI_Comm comm) */
        MPI_Reduce(MPI_IN_PLACE, &prod, 1, MPI_DOUBLE, MPI_PROD, 0, MPI_COMM_WORLD);
				printf("RESULT!!!\n");
    	  printf("%lf\n", prod);
    }
    else
    {
        MPI_Reduce(&prod, NULL, 1, MPI_DOUBLE, MPI_PROD, 0, MPI_COMM_WORLD);
    }

    /* Вивід результату */
    if(rank == 0)
    {
        printf("DET = %f\n", prod);
    }
    /* Повернення виділених ресурсів системі та фіналізація середовища MPI */
    free(MAh);
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
