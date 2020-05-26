#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* ОПИС ПРИКЛАДУ
 * 1. У кожній задачі зберігаються стовпці.
 */

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

/* Ім’я вхідного файлу */
const char *input_file_MA = "MA_2.txt";
const char *output_file_prefix = "out_";
const char *output_file_suffix = ".out";
/* Тег повідомленя, що містить стовпець матриці */
const int COLUMN_TAG = 0x1;
/* Основна функція (програма обчислення визначника) */
int main(int argc, char *argv[])
{
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
    }
		if (rank == 0) {
			printf("N = %d\n", N);
			printf("MATRIX MA\n");
		}

		matrix_print(MA);
		printf("\n");
    /* Розсилка всім задачам розмірності матриць та векторів */
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

		/* DEFINE TYPES !!! */
		/* Обчислення кількості стовпців !!!, які будуть зберігатися в кожній задачі та
    * виділення пам’яті для їх зберігання */
    int part = N / np;
		printf("PARTS = %d\n", part);
    struct my_matrix *MAh = matrix_alloc(N, part, .0);
    /* Створення та реєстрація типу даних для стовпця елементів матриці */
    MPI_Datatype matrix_columns;
    /* MPI_Type_vector (int count, int blockLength, int stride, MPI_Datatype, MPI_Datatype *newType) */
    MPI_Type_vector(N*part, 1, np, MPI_DOUBLE, &matrix_columns);
    MPI_Type_commit(&matrix_columns);
    /* Створення та реєстрація типу даних для структури вектора */
    MPI_Datatype vector_struct;
    MPI_Aint extent;
    MPI_Type_extent(MPI_INT, &extent); 		// визначення розміру в байтах
    MPI_Aint offsets[] = {0, extent};
    int lengths[] = {1, N+1};
    MPI_Datatype oldtypes[] = {MPI_INT, MPI_DOUBLE};
    /* MPI_Type_struct(int count, int blockLength[], MPI_Aint offsets[],
     * MPI_Datatype[] oldtypes, MPI_Datatype *newType) */
    MPI_Type_struct(2, lengths, offsets, oldtypes, &vector_struct);
    MPI_Type_commit(&vector_struct);
    /* Розсилка стовпців матриці з задачі 0 в інші задачі */
		/* DEFINE TYPES !!! */
		if (rank == 0) {
			for(int i = 1; i < np; i++)
			{
					printf("np = %d\n", np);
					printf("i = %d\n", i);
					printf("SEND MAh");
					for (int j = 0; j < MA->cols; j++) {
							printf("%f", MA->data[j * MA->rows + i]);
					}
			}
			printf("\n");
		}

    if(rank == 0)
    {
      	for(int i = 1; i < np; i++)
      	{
						/* int MPI_Send(void* buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm); */
            MPI_Send(&(MA->data[i]), 1, matrix_columns, i, COLUMN_TAG, MPI_COMM_WORLD);
      	}
     	  /* Копіювання елементів стовпців даної задачі */
    	  for(int i = 0; i < part; i++)
       	{
        int col_index = i*np;
        	    for(int j = 0; j < N; j++)
         	    {
                  MAh->data[j*part + i] = MA->data[j*N + col_index];
          	  }
        }
				printf("MAh on rank = 0\n");
				matrix_print(MAh);
      free(MA);
    }
    else
    {
        MPI_Recv(MAh->data, N*part, MPI_DOUBLE, 0, COLUMN_TAG,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

		if (rank == 1) {
			printf("MAh on rank = 1\n");
			matrix_print(MAh);
		}

	  /* Поточне значення вектору l_i */
    struct my_vector *current_l = vector_alloc(N, .0);
    /* Частина стовпців матриці L */
    struct my_matrix *MLh = matrix_alloc(N, part, .0);
    /* Основний цикл ітерації (кроки) */
    for(int step = 0; step < N-1; step++)
    {
        /* Вибір задачі, що містить стовпець з ведучім елементом та обчислення
    	   * поточних значень вектору l_i */
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
         /* Розсилка поточних значень l_i */
         /* int MPI_Bcast(void *buffer, int count, MPI_Datatype, int root, MPI_Comm comm);
          * [IN/OUT] buffer - покажчик буферу відправки (для задачі root), для інших -
          * покажчик буферу прийому.
          * count - кілкість елементів у буфері
          * root - ранг задачі, що відправляє дані
          *  */
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
    /* Обислення добутку елементів, які знаходяться на головній діагоналі
    * основної матриці (з урахуванням номеру стовпця в задачі) */
    double prod = 1.;
    for(int i = 0; i < part; i++)
    {
        int row_index = i*np + rank;
    	  prod *= MAh->data[row_index*part + i];
    }
		/* Згортка в одноранговому моді. Вивід буде тільки в задачі 1 */
		MPI_Allreduce(MPI_IN_PLACE, &prod, 1, MPI_DOUBLE, MPI_PROD, MPI_COMM_WORLD);
		/* Згортка добутків елементів головної діагоналі та вивід результату в задачі 0 */
    if(rank == 0)
    {
				// printf("L MATRIX\n");
				// matrix_print(MA);
				/* int MPI_Reduce(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
MPI_Op op, int root, MPI_Comm comm) */
        // MPI_Reduce(MPI_IN_PLACE, &prod, 1, MPI_DOUBLE, MPI_PROD, 0, MPI_COMM_WORLD);
        printf("RESULT!!!\n");
    	  printf("%lf\n", prod);
    }
    else
    {
        // MPI_Reduce(&prod, NULL, 1, MPI_DOUBLE, MPI_PROD, 0, MPI_COMM_WORLD);
    }
    /* Повернення виділених ресурсів */
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
