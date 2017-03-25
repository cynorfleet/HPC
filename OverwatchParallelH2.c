//			Christian and Johnny
//		Parallel Matrix Multiplication
//		Program will multiply 2 square matricies
//		and return result to a 3rd matrix.
//		User must set the MATRIX_SIZE variable
//
#include <stdlib.h>
#include <stdio.h>
#include </usr/include/mpi/mpi.h>
#include <timer.h>

const int MATRIX_SIZE = 640;
const int NUM_PROCESSES = 16;
double start, finish;

// purpose: prints the matrix to outfile
// requires: populated matrix
void matrix_print(int *matrix)
{
	int i, z;
	FILE *outfile;
	outfile = fopen("./OverwatchParallelOutput.txt", "w");
	for (i = 0; i < MATRIX_SIZE; i++)
	{
		for (z = 0; z < MATRIX_SIZE; z++)
			fprintf(outfile, " %d ", matrix[i*MATRIX_SIZE+z]);
		fprintf(outfile, "\n");
	}
	fclose(outfile);
}

// purpose: initializes a 1D matrix (array) and populates it with specified value
// requires: populated matrix
void matrix_init1D(int matrix[], int size, int value)
{
		int i;

		for(i = 0; i<size; i++)
			matrix[i] = value;
}

// purpose: multiplies two matricies together and stores product in result
// requires: 3 initialized matrix
void mat_Multi(int result[], int matA_local[], int matrix_B[], int size)
{
		int x, y, z;

		//START MATH HERE
		for(x = 0; x < MATRIX_SIZE / size; x++)
		{
			for(y = 0; y < MATRIX_SIZE; y++)
			{
				for(z = 0; z < MATRIX_SIZE; z++)
					result[x*MATRIX_SIZE + y] += matA_local[(x*MATRIX_SIZE) + z] * matrix_B[z * MATRIX_SIZE];
			}
		}
}

// MAIN //
int main (int argc, char *argv[])
{
	// gets the current time
	GET_TIME(start);
	// standard MPI vars for processor id and # of processors
	int rank, size, partition, matSize;
	// specifies the size of "matrix" we are working with
	matSize = MATRIX_SIZE * MATRIX_SIZE;

	// the matrix load between processors
	int matA_local[matSize/NUM_PROCESSES];
	// the matrix which we will be multiplying
	int matrix_B[matSize];
	// the local product of matrix A and B
	int result[matSize/NUM_PROCESSES];
	// the final form of the product of matrix A and B
	int combined[matSize];

		// allocates and initializes matrix B with the value 2 thru MATRIX_SIZE x MATRIX_SIZE elements (1Darray)
		matrix_init1D(matrix_B, MATRIX_SIZE * MATRIX_SIZE, 2);

		// start MPI scope
		MPI_Init(&argc, &argv);
		// Gets processor info
		MPI_Comm_rank (MPI_COMM_WORLD, &rank);
		MPI_Comm_size (MPI_COMM_WORLD, &size);
		// the amount of work to be done
		partition = (MATRIX_SIZE * MATRIX_SIZE)/size;

		// initializes the local matix A and the local result matrix
		matrix_init1D(result, partition, 0);
		matrix_init1D(matA_local, partition, 1);

		// multiplies the 2 matricies A and B and stores in result
		mat_Multi(result, matA_local, matrix_B, size);

		// master grabs all the sub-products and combines to make final result matrix
		MPI_Gather(result, partition, MPI_INT, combined, partition, MPI_INT, 0, MPI_COMM_WORLD);
		// master prints the matrix
		if(rank == 0)
			matrix_print(combined);

	MPI_Finalize();

	// gets current time and calculates how long program took
	GET_TIME(finish);
	printf("Runtime: %f\n", finish-start);
	return 0;
}
