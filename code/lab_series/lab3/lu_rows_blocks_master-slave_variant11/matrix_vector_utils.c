#include <stdio.h>
#include <stdlib.h>

struct Vector {
    int size;
    int* data;
};

struct Matrix
{
    int rows;
    int cols;
    int **data;
};

struct Vector* read_vector(const char* filename)
{
  FILE *pf;
  pf = fopen(filename, "r");
  struct Vector* vector = malloc(sizeof(struct Vector));

  fscanf(pf, "%d", &vector->size);
  vector->data = (int *)malloc(vector->size * sizeof(int *));

  for(int i = 0; i < vector->size; i++)
  {
      fscanf(pf, "%d", &vector->data[i]);
  }
  fclose(pf);
  return vector;
}

void print_vector(struct Vector* vector)
{
  for(int i = 0; i < vector->size; i++)
  {
    printf("%d  ", vector->data[i]);
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
  matrix->data = (int **)malloc(matrix->rows * sizeof(int *));
      for (int i=0; i<matrix->rows; i++)
           matrix->data[i] = (int *)malloc(matrix->cols * sizeof(int));

  for(int i = 0; i < matrix->rows; i++)
  {
     for(int j = 0; j < matrix->cols; j++)
     {
         fscanf(pf, "%d", &matrix->data[i][j]);
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
        printf("%d  ", matrix->data[i][j]);
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
