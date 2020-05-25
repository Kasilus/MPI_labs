#include <stdio.h>
#include <mpi.h>
int main(int argc, char *argv[])
{
	/* Ініціалізація середовища MPI */
	MPI_Init(&argc, &argv);
	/* Отримання рангу даної задачі */
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	/* Отримання загальної кількості задач */
	int np;
	MPI_Comm_size(MPI_COMM_WORLD, &np);
	/* Кожна задача виконає цей оператор */
	printf("I am task %d. There are %d tasks total.\n", rank, np);
	if(rank == 1)
	{
	/* Цей оператор виконає тільки задача з рангом 1 */
	printf("I am task 1 and I do things differently.\n");
	}
	/* Де-ініціалізація середовища MPI та вихід з програми */
	MPI_Finalize();
	return 0;
}
