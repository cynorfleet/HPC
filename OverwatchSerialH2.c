//			Christian and Johnny
//		Serial Matrix Multiplication
//		Program will multiply 2 square matricies
//		and return result to a 3rd matrix.
//		User must set the MATRIX_SIZE variable
//
#include <stdlib.h>
#include <stdio.h>
#include <timer.h>

// specifies the size of square matrix (n x n)
const int MATRIX_SIZE = 640;
double start, finish;

// purpose: initializes an matrix with passed in value.
// requires: dynamically declared 2d array 
// returns: matrix via pass by reference
int matrix_init(int*** matrix, int size, int value)
{
//	variables
	int count_x, count_y;
	int** temp_matrix;

//	declare dynamic array of pointers
	temp_matrix = malloc(size * sizeof(int*));
//	itterate through rows and init with columns
	for (count_x = 0; count_x < size; count_x++)
	{
//		declare column array here
		temp_matrix[count_x] = malloc(size * sizeof(int));
//		sets element values
		for (count_y = 0; count_y < size; count_y++)
		{
				temp_matrix[count_x][count_y] = value;
		}
	}
//	matrix via pass by refrence
	*matrix = temp_matrix;
}


// purpose: prints the matrix to screen 
// requires: populated matrix 
void matrix_print(int **matrix)
{
	FILE *outfile;
	outfile = fopen("./OverwatchSerialOutput.txt", "w");
	int count_x, count_y;
	for (count_x = 0; count_x <  MATRIX_SIZE; count_x++)
	{
		for (count_y = 0; count_y < MATRIX_SIZE; count_y++)
			fprintf(outfile, "%d ", matrix[count_x][count_y]);
		fprintf(outfile, "\n");
	}
}


// purpose: deletes the 2d Array
// requires: populated matrix 
void matrix_free(int **matrix, int row_size)
{
	int count_x;
    	for (count_x = 0; count_x < row_size; count_x++)
    	{
        	free(matrix[count_x]);
    	}
    	free(matrix);
}

// purpose: multiplies Matrix A row by Matrix B column
// requires: populated matrix 
// returns: matrix via pass by reference
void matrix_multi(int **matrix_A, int **matrix_B, int ***result, int size)
{
	int count_x, count_y, count_z;
	int **temp_matrix;

	matrix_init(&temp_matrix, MATRIX_SIZE, 0);

	for(count_x = 0; count_x < size; count_x++)
	{
		for(count_y = 0; count_y < size; count_y++)
		{
			for(count_z = 0; count_z < size; count_z++)
				temp_matrix[count_x][count_y]+= matrix_A[count_x][count_z] * matrix_B[count_z][count_y];
		}
	}
	*result = temp_matrix;
}

// MAIN //
int main ()
{
	GET_TIME(start);
	int **matrix_A, **matrix_B, **result;
	
	matrix_init(&matrix_A, MATRIX_SIZE, 1);
	matrix_init(&matrix_B, MATRIX_SIZE, 2);

	matrix_multi(matrix_A, matrix_B, &result, MATRIX_SIZE);

	matrix_print(result);

	matrix_free(matrix_A, MATRIX_SIZE);
	matrix_free(matrix_B, MATRIX_SIZE);
	matrix_free(result, MATRIX_SIZE);

	GET_TIME(finish);
	printf("\nruntime: %f", finish - start);

	return 0;
}
