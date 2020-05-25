#include <stdio.h>
#include <mpi.h>

const int TAG_DATA1 = 10;
const int TAG_DATA2 = 20;
const int TAG_DATA3 = 25;

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0)
  {
    int x;
    MPI_Recv(&x, 1, MPI_INT, MPI_ANY_SOURCE, TAG_DATA3, MPI_COMM_WORLD,
    MPI_STATUS_IGNORE);
    printf("Got %d with tag TAG_DATA3\n", x);
    MPI_Recv(&x, 1, MPI_INT, MPI_ANY_SOURCE, TAG_DATA1, MPI_COMM_WORLD,
    MPI_STATUS_IGNORE);
    printf("Got %d with tag TAG_DATA1\n", x);
    MPI_Recv(&x, 1, MPI_INT, MPI_ANY_SOURCE, TAG_DATA2, MPI_COMM_WORLD,
    MPI_STATUS_IGNORE);
    printf("Got %d with tag TAG_DATA2\n", x);
  }
  else if (rank == 1)
  {
    int x = 1000;
    MPI_Send(&x, 1, MPI_INT, 0, TAG_DATA1, MPI_COMM_WORLD);
  }
  else if (rank == 2)
  {
    int x = 2000;
    MPI_Send(&x, 1, MPI_INT, 0, TAG_DATA2, MPI_COMM_WORLD);
  }
  else
  {
    int x = 3000;
    MPI_Send(&x, 1, MPI_INT, 0, TAG_DATA3, MPI_COMM_WORLD);
  }
  MPI_Finalize();
  return 0;
}
