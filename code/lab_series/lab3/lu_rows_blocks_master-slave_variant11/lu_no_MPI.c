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
    struct Matrx *l = matrix_alloc(N, N, .0);
    for (int k = 0; k < N - 1; k++)
    {
        for (int i = k + 1; i < N; i++)
        {
            /* L */
            l->data[i * l->cols + k] = MA->data[i * MA->cols + k] /
              MA->data[k * MA->cols + k];
            for (int j = k + 1; j < N; j++)
            {
                /* MA = U */
                MA->data[i * l->cols + j] = MA->data[i * l->cols + j] -
                  l->data[i * l->cols + k] * MA->data[k * l->cols + j];
            }
        }
    }
    /* det(LU) = det(L) * det(U) = det(U) */
    for (int i = 0; i < N * N; i++)
    {
        if (MA->data[i] != 0)
        {
            det_inner *= MA->data[i];
        }
    }
    *det = det_inner;
}
/* Основна функція */
int main(int argc, char *argv[])
{
    const char *input_file_MA = "MA.txt";
    const char *input_file_b = "b.txt";
    const char *output_file_x = "det.txt";
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
