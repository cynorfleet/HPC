#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main (void)
{
	const int MAX_NUM = 10000;
	int rank, size;

	// entering parallel scoping
	MPI_Init (NULL, NULL);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);


	//								MASTER								//
	if(rank == 0)
	{
	int WORK = MAX_NUM / size;
	int i = 0, x = 0;
		// allocate slave array
        	// allocate masters array

		int * summation = (int *) malloc(WORK * sizeof(int));
		int * locarry = (int *) malloc(WORK * sizeof(int));

		// initialize array for distributed work
		for(x = 0; x < WORK; x++)
		{
			locarry[x * WORK] = x + 1;
		}
		

		// send array partition to slaves

		// sum the local array
		for(i = 0; i < WORK; i++)
		{
			locarry[0] += summation[i];
		}
		int localsum = 0;
		// get data FROM everyone BUT SELF!!!!
			MPI_Gather(&localsum, 1, MPI_INT, locarry, size, MPI_INT, rank, MPI_COMM_WORLD);

		// do final summation of results from processes
		for(i = 0; i < size; i++)
		{
			localsum += locarry[i];
		}
			printf("FINAL SUM = %d\n", localsum);
	}

	//								SLAVES								//
	if(rank != 0)
	{
	int WORK = MAX_NUM / size;
	int i = 0, x = 0;
		int * summation = (int *) malloc(WORK * sizeof(int));
		int * locarry = (int *) malloc(WORK * sizeof(int));

		// initialize array for distributed work
		for(x = 0; x < WORK; x++)
		{
			locarry[x * WORK] = x + 1;
		}
		// init array for partitioned size
		// slaves recieve array from master

		// do summation of arrays
		int localsum = 0;
		for(i = 0; i < WORK; i++)
		{
			localsum += locarry[i];
		}

		// send local sums to master
		MPI_Bcast(&localsum, 1, MPI_INT, rank, MPI_COMM_WORLD);
	}
	// leaving parallel scoping
	MPI_Finalize();
	return 0;
}
