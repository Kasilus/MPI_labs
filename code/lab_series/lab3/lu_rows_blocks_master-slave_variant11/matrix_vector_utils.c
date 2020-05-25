#include <stdio.h>
#include <stdlib.h>

struct Vector {
    int size;
    double* data;
};

struct Matrix
{
    int rows;
    int cols;
    double** data;
};

struct Vector* read_vector(const char* filename)
{
  FILE *pf;
  pf = fopen(filename, "r");
  struct Vector* vector = malloc(sizeof(struct Vector));

  fscanf(pf, "%d", &vector->size);
  vector->data = (double *)malloc(vector->size * sizeof(double *));

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
  struct Matrix* matrix = malloc(sizeof(struct Matrix));

  fscanf(pf, "%d", &matrix->rows);
  fscanf(pf, "%d", &matrix->cols);
  matrix->data = (double **)malloc(matrix->rows * sizeof(double *));
  for (int i=0; i<matrix->rows; i++)
       matrix->data[i] = (double *)malloc(matrix->cols * sizeof(double));

  for(int i = 0; i < matrix->rows; i++)
  {
     for(int j = 0; j < matrix->cols; j++)
     {
         fscanf(pf, "%lf", &matrix->data[i][j]);
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
        printf("%f  ", matrix->data[i][j]);
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
