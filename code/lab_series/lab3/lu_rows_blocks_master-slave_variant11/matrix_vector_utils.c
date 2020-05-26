#include <stdio.h>
#include <stdlib.h>

struct Vector {
    int size;
    double data[1];
};

struct Matrix
{
    int rows;
    int cols;
    double data[1];
};

struct Vector* read_vector(const char* filename)
{
  FILE *pf;
  pf = fopen(filename, "r");

  int size;
  fscanf(pf, "%d", &size);
  struct Vector *vector = vector_alloc(size, 0.0);

  for(int i = 0; i < vector->size; i++)
  {
      fscanf(pf, "%lf", &vector->data[i]);
  }
  fclose(pf);
  return vector;
}

void print_vector(struct Vector* vector)
{
  for(int i = 0; i < vector->size; i++)
  {
    printf("%lf  ", vector->data[i]);
  }
  printf("\n");
}

struct Matrix* read_matrix(const char* filename)
{
  FILE *pf;
  pf = fopen(filename, "r");

  int rows, cols;
  fscanf(pf, "%d", &rows);
  fscanf(pf, "%d", &cols);
  struct Matrix *result = matrix_alloc(rows, cols, 0.0);

  for(int i = 0; i < matrix->rows; i++)
  {
     for(int j = 0; j < matrix->cols; j++)
     {
         fscanf(pf, "%lf", &matrix->data[i * cols + j]);
     }
  }
  fclose(pf);
  return matrix;
}

void print_matrix(struct Matrix* matrix)
{
  for(int i = 0; i < matrix->rows; i++)
  {
     for(int j = 0; j < matrix->cols; j++)
     {
        printf("%f  ", matrix->data[i * cols + j]);
     }
     printf("\n");
  }
}

void write(const char* filename, double value)
{
  FILE *pf;
  pf = fopen(filename, "w");
  fprintf(pf ,"%f ", value);
  fclose(pf);
}

struct Vector *vector_alloc(int size, double initial)
{
  	struct Vector *result = (struct  Vector *) malloc(sizeof(struct Vector) + (size - 1) * sizeof(double));
  	result->size = size;

  	for (int i = 0; i < size; i++)
  	{
  		result->data[i] = initial;
  	}
  	return result;
}

struct Matrix *matrix_alloc(int rows, int cols, double initial)
{
    struct Matrix *result = (struct  Matrix *) malloc(sizeof(struct Matrix) + (rows * cols - 1) * sizeof(double));
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

void fatal_error(const char *message, int errorcode)
{
  	printf("fatal error: code %d, %s\n", errorcode, message);
  	fflush(stdout);
  	MPI_Abort(MPI_COMM_WORLD, errorcode);
}
