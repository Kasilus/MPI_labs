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

struct Vector* read_vector(const char* filename);
void print_vector(struct Vector* vector);
struct Matrix* read_matrix(const char* filename);
void print_matrix(struct Matrix* matrix);
void write(const char* filename, double value);
