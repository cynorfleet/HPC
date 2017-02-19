#include </usr/lib/mpich/include/mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char* argv[])
{	// convert char to int
	const int MAX_NUM = atoi(argv[1]); 
	int rank, size;
	int * arry;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

	int WORK = MAX_NUM / size;
	
	if(rank == 0)
	{
		printf("WORLD SIZE is %d\n", size);
		printf("MAX_NUM is %d\n", MAX_NUM);
		printf("WORK is %d\n", WORK);
		printf("COMM_SIZE is %d\n\n", size);
		fflush(stdout);
	}

		// allocate slave arrays
		int offset = rank * WORK;
		arry = (int *) malloc(WORK * sizeof(int));
		
		// initialize array for distributed work
		for(int x = 0; x < WORK; x++)
		{
			arry[x] = x + (offset) + 1;
			printf("arry[%d] of process %d is %d\n", x, rank, arry[x]);
			fflush(stdout);
		}

		printf("\n\n");
		fflush(stdout);

		// do the reduction
		int localsum = 0;
		for(int i = 0; i < WORK; i++)
		{
			localsum += arry[i];
		}

		// sync data to master
		if( rank != 0 )
		{
			MPI_Send(&localsum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}

		// grab data from slaves
		if( rank == 0)
		{
			int remotedata = 0;
			// get data FROM everyone BUT SELF!!!!
			for(int i = 1; i < size; i++)
			{
				MPI_Recv(&remotedata, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				// complete summation
				localsum += remotedata;
			}
		}

	// Print results
	for(int i = 0; i < size; i++)
	{
		if( rank == i )
		{
			printf("The sum from process %d is %d\n", rank, localsum);
			fflush(stdout);
		}
	}

	if( rank == 0)
	{
		printf("FINAL SUM = %d\n", localsum);
		fflush(stdout);
	}
	MPI_Finalize();
	return 0;
}
