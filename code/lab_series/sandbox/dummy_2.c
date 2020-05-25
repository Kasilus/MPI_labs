#include <stdio.h>
#include <stdlib.h>

struct Matrix {
    int rows;
    int cols;
    int *data;
};

struct Matrix* read_matrix(const char* filename)
{
  FILE *pf;
  pf = fopen(filename, "r");
  // if (pf == NULL)
  //     return 0;
  int r, c;
  fscanf(pf, "%d", &r);
  fscanf(pf, "%d", &c);
  struct Matrix* matrix = malloc(sizeof(struct Matrix));
  matrix->rows = r;
  matrix->cols = c;
  matrix->data = malloc(sizeof(int) * r * c);

  for(int i = 0; i < r * c; i++)
  {
      fscanf(pf, "%d", &matrix->data[i]);
  }

  fclose(pf);
  return matrix;
}

int main()
{
   struct Matrix* MA = read_matrix("MA.txt");
   printf("MA\n");
   for(int i = 0; i < MA->rows * MA->cols; i++)
   {
      printf("%d  ", MA->data[i]);
   }
   free(MA);

   // int* b;
   // pf = fopen("b.txt", "r");
   // if (pf == NULL)
   //     return 0;
   // fscanf(pf, "%d", &r);
   // b = (int *)malloc(r * sizeof(int *));
   //
   // for(int i = 0; i < r; i++)
   // {
   //    fscanf(pf, "%d", &b[i]);
   // }
   // printf("b\n");
   // for(int i = 0; i < r; i++)
   // {
   //    printf("%d  ", b[i]);
   // }
   // printf("\n");
   // free(b);
   //
   // double det = 962.312;
   // pf = fopen("det.txt", "w");
   // fprintf(pf ,"%f ", det);

   return 0;
}
