#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main (void)
{	// convert char to int
	const int MAX_NUM = 10000;
	int rank, size;

	MPI_Init (NULL, NULL);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	int * locarry;
	int * arry;
	int WORK = MAX_NUM / size;
	int i = 0;
	int localsum = 0;

	if(rank == 0)
	{
		// allocate master's arrays
		int * locarry = (int *) malloc(WORK * sizeof(int));
		arry = (int *) malloc(MAX_NUM * sizeof(int));

		// initialize array for distributed work
		for(i = 0; i < MAX_NUM; i++)
		{
			arry[i] = i + 1;
		}
	}

	// allocate slave arrays
	if(rank != 0)
	{
		// init array for partitioned size
		arry = (int *) malloc(WORK);
	}

	// master sends array to slaves
	if(rank == 0)
	{
		for(i = 1; i < size-1; i++)
		MPI_Send((arry + (i * WORK)), WORK, MPI_INT, i, 0, MPI_COMM_WORLD);
	}

	// slaves recieve array from master
	if(rank > 0)
	{
			MPI_Recv(arry, WORK, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

		// EVERYONE does the reduction
		for(i = 0; i < WORK; i++)
		{
			localsum += arry[i];
		}

		// sync data to master
		if( rank > 0 )
		{
			// DEBUG **************************
			MPI_Send(&localsum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}

		// grab data from slaves
		if( rank == 0)
		{
			locarry[0] = localsum;
			// get data FROM everyone BUT SELF!!!!
			for(i = 1; i < size-1; i++)
			{
				MPI_Recv(locarry + i, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				// complete summation
				locarry[i] = localsum;
			}

		for(i = 0; i < size-1; i++)
		{
			printf("The sum from process %d is %d\n", i, locarry[i]);
			if(i>0)
			{
				localsum += locarry[i];
			}
		}
			printf("FINAL SUM = %d\n", localsum);
			fflush(stdout);
	}
	MPI_Finalize();
	return 0;
}
