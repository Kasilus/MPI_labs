#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix_vector_utils.h"

void lu(
    struct Matrix* MA,
    struct Vector* b,
    double* det)
{
    double det_inner = 1.0;
    int N = b->size;
    // move to another method for matrix allocation
    struct Matrix *l = malloc(sizeof(struct Matrix));
    l->rows = N;
    l->cols = N;
    l->data = (double **)malloc(l->rows * sizeof(double *));
    for (int i=0; i<l->rows; i++)
         l->data[i] = (double *)malloc(l->cols * sizeof(double));
    // fill with 0 TO DELETE
    for (int i=0; i<l->rows; i++) {
        for (int j=0; j<l->cols; j++) {
            l->data[i][j] = 0;
        }
    }
    printf("Matrix l after init\n");
    print_matrix(l);
    for (int k = 0; k < N - 1; k++)
    {
        for (int i = k + 1; i < N; i++)
        {
            /* L */
            l->data[i][k] = MA->data[i][k] / MA->data[k][k];
            for (int j = k + 1; j < N; j++)
            {
                /* MA = U */
                MA->data[i][j] = MA->data[i][j] -
                  l->data[i][k] * MA->data[k][j];
            }
        }
    }
    printf("Matrix L\n");
    print_matrix(l);
    printf("Matrix U\n");
    print_matrix(MA);
    /* det(LU) = det(L) * det(U) = det(U) */
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++) {
            if (MA->data[i] != 0)
              {
                  det_inner *= MA->data[i][j];
              }
        }
    }
    printf("det(LU)");
    printf("%f\n", det_inner);
    *det = det_inner;
}
/* Основна функція */
int main(int argc, char *argv[])
{
    const char *input_file_MA = "MA.txt";
    const char *input_file_b = "b.txt";
    const char *output_file_x = "det.out";
    /* Зчитування даних в задачі 0 */
    struct Matrix* MA = read_matrix(input_file_MA);
    struct Vector* b = read_vector(input_file_b);
    if(MA->rows != MA->cols) {
        printf("Matrix MA is not square!");
        return -1;
    }
    printf("Matrix MA\n");
    print_matrix(MA);
    if(MA->rows != b->size) {
        printf("Dimensions of matrix and vector don’t match!");
        return -1;
    }
    printf("Vector b\n");
    print_vector(b);
    double det;
    lu(MA, b, &det);
    write(output_file_x, det);
    free(MA);
    free(b);
    return 0;
}
