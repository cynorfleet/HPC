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
