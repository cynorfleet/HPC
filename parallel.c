//			Christian and Johnny
//		Serial Matrix Multiplication
//		Program will multiply 2 square matricies
//		and return result to a 3rd matrix.
//		User must set the MATRIX_SIZE variable
//
#include <stdlib.h>
#include <stdio.h>
#include </usr/include/mpi/mpi.h>


// specifies the size of square matrix (n x n)
const int MATRIX_SIZE = 3;

// purpose: prints the matrix to screen 
// requires: populated matrix 
void matrix_print(int *matrix)
{
	int i, z;
	for (i = 0; i < MATRIX_SIZE; i++)
	{
		printf("\nrow %d: ", i);
		for (z = 0; z < MATRIX_SIZE; z++)
			printf(" %d ", matrix[i*MATRIX_SIZE+z]);
	}
	printf("\n");
}

void matrix_init1D(int matrix[], int size, int value)
{
		int i;
		int sz = size*size;


		// declare dynamic array of pointers
		for(i = 0; i<sz; i++)
			matrix[i] = value;
}

// MAIN //
int main (int argc, char *argv[])
{
	int rank, size, partition;
	partition = (MATRIX_SIZE * MATRIX_SIZE)/size;
	int matrix_A[640*640] ={};
	int matA_local[640*640/16] = {};
	int matrix_B[640*640]= {};
	int result[640*640/16]={};
	int combined[640*640]={};

	printf("STARTING!!!\n");
	
		matrix_init1D(matrix_B, MATRIX_SIZE, 2);

		MPI_Init(&argc, &argv);

		MPI_Comm_rank (MPI_COMM_WORLD, &rank);
		MPI_Comm_size (MPI_COMM_WORLD, &size);

		if (rank == 0)
			// init matA and result
			matrix_init1D(matrix_A, MATRIX_SIZE, 1);
		
		matrix_init1D(result, MATRIX_SIZE, 0);

		MPI_Scatter(matrix_A, (MATRIX_SIZE*MATRIX_SIZE)/size, MPI_INT, matA_local, (MATRIX_SIZE*MATRIX_SIZE)/size, MPI_INT, 0, MPI_COMM_WORLD);

		int x, y;

		//START MATH HERE
		for(x = 0; x < partition; x++)
		{
			for(y = 0; y < MATRIX_SIZE; y++)
			{
				result[(x*MATRIX_SIZE)+y] += matrix_A[(x*MATRIX_SIZE) + y] * matrix_B[y * MATRIX_SIZE];
			}
		}

		MPI_Gather(result, partition, MPI_INT, combined, partition, MPI_INT, 0, MPI_COMM_WORLD);

		matrix_print(combined);
		MPI_Finalize();

	printf("\nDONE!");

	return 0;
}
