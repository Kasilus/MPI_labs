#include <stdio.h>
#include <stdlib.h>

int readmatrix(int** MA, const char* filename)
{
    FILE *pf;
    pf = fopen (filename, "r");
    if (pf == NULL)
        return 0;

    int r, c;
    fscanf(pf, "%d", &r);
    fscanf(pf, "%d", &c);
    MA = (int **)malloc(r * sizeof(int *));
        for (int i=0; i<r; i++)
             MA[i] = (int *)malloc(c * sizeof(int));

    for(int i = 0; i < r; i++)
    {
        for(int j = 0; j < c; j++)
        {
            fscanf(pf, "%d", &MA[i][j]);
        }
    }

    fclose (pf);
    return 1;
}

int main()
{
   int** MA;

   FILE *pf;
   pf = fopen("MA.txt", "r");
   if (pf == NULL)
       return 0;
   int r, c;
   fscanf(pf, "%d", &r);
   fscanf(pf, "%d", &c);
   MA = (int **)malloc(r * sizeof(int *));
       for (int i=0; i<r; i++)
            MA[i] = (int *)malloc(c * sizeof(int));

   for(int i = 0; i < r; i++)
   {
       for(int j = 0; j < c; j++)
       {
           fscanf(pf, "%d", &MA[i][j]);
       }
   }
   printf("MA\n");
   for(int i = 0; i < r; i++)
   {
       for(int j = 0; j < c; j++)
       {
           printf("%d  ", MA[i][j]);
       }
       printf("\n");
   }
   free(MA);

   int* b;
   pf = fopen("b.txt", "r");
   if (pf == NULL)
       return 0;
   fscanf(pf, "%d", &r);
   b = (int *)malloc(r * sizeof(int *));

   for(int i = 0; i < r; i++)
   {
      fscanf(pf, "%d", &b[i]);
   }
   printf("b\n");
   for(int i = 0; i < r; i++)
   {
      printf("%d  ", b[i]);
   }
   printf("\n");
   free(b);

   double det = 962.312;
   pf = fopen("det.txt", "w");
   fprintf(pf ,"%f ", det);

   return 0;
}
