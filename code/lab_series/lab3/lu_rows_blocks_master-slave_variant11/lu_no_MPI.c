#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix_vector_utils.h"

void lu(
    struct Matrix* MA,
    double* det)
{
    double det_inner = 1.0;
    int N = MA->cols;
    // move to another method for matrix allocation
    struct Matrix *l = matrix_alloc(N, N, .0);
    for (int k = 0; k < N - 1; k++)
    {
        for (int i = k + 1; i < N; i++)
        {
            /* L */
            printf("k, i = %d, %d\n", k, i);
            printf("MA[%d][%d] %f\n", (i+1), (k+1), MA->data[i][k]);
            printf("MA[%d][%d] %f\n", (k+1), (k+1), MA->data[k][k]);
            l->data[i][k] = MA->data[i][k] / MA->data[k][k];
            printf("NEW L[%d][%d] %f\n", (i+1), (k+1), l->data[i][k]);
            for (int j = k + 1; j < N; j++)
            {
                /* MA = U */
                printf("k, i, j = %d, %d, %d\n", k, i, j);
                printf("k, i = %d, %d\n", k, i);
                printf("MA[%d][%d] %f\n", (i+1), (j+1), MA->data[i][j]);
                printf("l[%d][%d] %f\n", (i+1), (k+1), l->data[i][k]);
                printf("MA[%d][%d] %f\n", (k+1), (j+1), MA->data[k][j]);
                MA->data[i][j] = MA->data[i][j] -
                  l->data[i][k] * MA->data[k][j];
                printf("NEW MA[%d][%d] %f\n", (i+1), (j+1), MA->data[i][j]);
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
            if (i == j)
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
    const char *input_file_MA = "MA_2.txt";
    const char *output_file_x = "det.out";
    /* Зчитування даних в задачі 0 */
    struct Matrix* MA = read_matrix(input_file_MA);
    if(MA->rows != MA->cols) {
        printf("Matrix MA is not square!");
        return -1;
    }
    printf("Matrix MA\n");
    print_matrix(MA);
    double det;
    lu(MA, &det);
    write(output_file_x, det);
    free(MA);
    return 0;
}
