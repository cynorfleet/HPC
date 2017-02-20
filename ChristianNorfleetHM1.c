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

	int WORK = MAX_NUM / size;
	int i = 0, x = 0;

	//								MASTER								//
	if(rank == 0)
	{
		// allocate slave array
		int * locarry = (int *) malloc(WORK * sizeof(int));
	  // allocate masters array
		int * summation = (int *) malloc(MAX_NUM * sizeof(int));

		// initialize array for distributed work
		for(x = 0; x < MAX_NUM; x++)
		{
			summation[x] = x + 1;
		}

		// send array partition to slaves
		for(i = 1; i < size; i++)
			MPI_Send((summation + (i * WORK)), WORK, MPI_INT, i, 0, MPI_COMM_WORLD);

		// sum the local array
		for(i = 0; i < WORK; i++)
		{
			locarry[0] += summation[i];
		}
		// get data FROM everyone BUT SELF!!!!
		for(i = 1; i < size; i++)
		{
			MPI_Recv(locarry + i, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		// do final summation of results from processes
		int localsum = 0;
		for(i = 0; i < size; i++)
		{
			printf("The local sum of process %d = %d\n", i, locarry[i]);
			localsum += locarry[i];
		}
			printf("FINAL SUM = %d\n", localsum);
	}

	//								SLAVES								//
	if(rank != 0)
	{
		// init array for partitioned size
		int * locarry = (int *) malloc(WORK * sizeof(int));
		// slaves recieve array from master
		MPI_Recv(locarry, WORK, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		// do summation of arrays
		int localsum = 0;
		for(i = 0; i < WORK; i++)
		{
			localsum += locarry[i];
		}

		// send local sums to master
		MPI_Send(&localsum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	// leaving parallel scoping
	MPI_Finalize();
	return 0;
}
