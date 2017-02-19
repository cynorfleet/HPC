#include </usr/lib/mpich/include/mpi.h>
#include <stdio.h>

void DisplayArg(int argc, char* argv[])
{
	printf("There are %d arguments", argc);
	for(int i = 0; i < argc; i++)
	{
		printf("argv[%d] = %s\n", i, argv[i]);
	}
}

int main (int argc, char* argv[])
{
	DisplayArg(argc, argv);
	int rank, size;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	printf("Hello world from process %d of %d\n", rank, size);
	MPI_Finalize();
	return 0;
}
