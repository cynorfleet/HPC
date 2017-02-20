#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main (void)
{	// convert char to int
	const int MAX_NUM = 100;
	int rank, size;

	MPI_Init (NULL, NULL);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	int * locarry;
	int * arry;
	int WORK = MAX_NUM / size;
	int i = 0, x = 0;

	// allocate masters arrays
	if(rank == 0)
	{
		locarry = (int *) malloc(WORK * sizeof(int));
		arry = (int *) malloc(MAX_NUM * sizeof(int));

		printf("WORLD SIZE is %d\n", size);
		printf("MAX_NUM is %d\n", MAX_NUM);
		printf("WORK is %d\n", WORK);
		printf("COMM_SIZE is %d\n\n", size);


		// initialize array for distributed work
		for(x = 0; x < MAX_NUM; x++)
		{
			arry[x] = x + 1;
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
		for(i = 1; i < size; i++)
		MPI_Send((arry + (i * WORK)), ( WORK), MPI_INT, i, 0, MPI_COMM_WORLD);
	}

	// slaves recieve array from master
	if(rank != 0)
	{
			MPI_Recv(arry, (WORK), MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

		// EVERYONE does the reduction
		int localsum = 0;
		for(i = 0; i < WORK; i++)
		{
			localsum += arry[i];
		}
			// DEBUG **************************
		  printf("Localsum process: %d is %d\n", rank, localsum);

		// sync data to master
		if( rank != 0 )
		{
			// DEBUG **************************
			MPI_Send(&localsum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}

		// grab data from slaves
		if( rank == 0)
		{
			int remotedata = 0;
			locarry[0] = localsum;
			// get data FROM everyone BUT SELF!!!!
			for(i = 1; i < size; i++)
			{
				MPI_Recv(&remotedata, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				// complete summation
				locarry[i] = remotedata;
			}
			// DEBUG **************************
		  printf("SEND RECIEVE COMPLETE");
		}

	// Print results
	for(i = 0; i < size; i++)
	{
		if( rank == i )
		{
			printf("The sum from process %d is %d\n", i, locarry[i]);
		}
	}

	if( rank == 0)
	{
		for(i = 1; i < size; i++)
		{
			localsum += locarry[i];
		}
			printf("FINAL SUM = %d\n", localsum);
	}
	MPI_Finalize();
	return 0;
}
