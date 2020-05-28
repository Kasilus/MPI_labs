#ifndef MATRIX_VECTOR_UTILS_H
#define MATRIX_VECTOR_UTILS_H

struct Vector
{
    int size;
    double data[1];
};

struct Matrix
{
    int rows;
    int cols;
    double data[1];
};

struct Vector *vector_alloc(int size, double init);
struct Matrix *matrix_alloc(int rows, int cols, double init);
struct Vector *read_vector(const char *filename);
void print_vector(struct Vector *vector);
struct Matrix *read_matrix(const char *filename);
void print_matrix(struct Matrix *matrix);

#endif
