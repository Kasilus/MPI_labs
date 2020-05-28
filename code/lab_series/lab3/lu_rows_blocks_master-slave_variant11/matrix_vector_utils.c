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

struct Vector *vector_alloc(int size, double init)
{
  	struct Vector *vector = (struct  Vector *) malloc(sizeof(struct Vector) + (size - 1) * sizeof(double));
  	vector->size = size;

  	for (int i = 0; i < size; i++)
  	{
  		vector->data[i] = init;
  	}
  	return vector;
}

struct Matrix *matrix_alloc(int rows, int cols, double init)
{
    struct Matrix *matrix = (struct Matrix *) malloc(sizeof(struct Matrix) + (rows * cols - 1) * sizeof(double));
    matrix->rows = rows;
    matrix->cols = cols;

  	for (int i = 0; i < rows; i++)
  	{
  		for (int j = 0; j < cols; j++)
  		{
  			matrix->data[i * cols + j] = init;
  		}
  	}
  	return matrix;
}

struct Vector *read_vector(const char *filename)
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

void print_vector(struct Vector *vector)
{
  for(int i = 0; i < vector->size; i++)
  {
    printf("%lf  ", vector->data[i]);
  }
  printf("\n");
}

struct Matrix *read_matrix(const char *filename)
{
  FILE *pf;
  pf = fopen(filename, "r");

  int rows, cols;
  fscanf(pf, "%d", &rows);
  fscanf(pf, "%d", &cols);
  struct Matrix *matrix = matrix_alloc(rows, cols, 0.0);

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

void print_matrix(struct Matrix *matrix)
{
  for(int i = 0; i < matrix->rows; i++)
  {
     for(int j = 0; j < matrix->cols; j++)
     {
        printf("%f  ", matrix->data[i * matrix->cols + j]);
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
