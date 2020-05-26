struct Vector {
    int size;
    double[1] data;
};

struct Matrix
{
    int rows;
    int cols;
    double[1] data;
};

struct Vector* read_vector(const char* filename);
void print_vector(struct Vector* vector);
struct Matrix* read_matrix(const char* filename);
void print_matrix(struct Matrix* matrix);
void write(const char* filename, double value);
struct Vector *vector_alloc(int size, double initial);
struct Matrix *matrix_alloc(int rows, int cols, double initial);
void fatal_error(const char *message, int errorcode);
