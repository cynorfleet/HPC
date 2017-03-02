#include <stdlib.h>
#include <stdio.h>

const int MATRIX_SIZE = 10;

int matrix_init(int*** matrix, int size, int value)
{

	int count = 0;
	int count_x, count_y;
	int** temp_matrix;

	temp_matrix = malloc(size * sizeof(int*));
	for (int count_x = 0; count_x < size; count_x++)
	{
		temp_matrix[count_x] = malloc(size * sizeof(int));
		for (int count_y = 0; count_y < size; count_y++)
		{
			if(value !=0)
				temp_matrix[count_x][count_y] = count++;
			else
				temp_matrix[count_x][count_y] = 0;
		}
	}
	*matrix = temp_matrix;
}

void matrix_debug(int **matrix)
{
	int count_x, count_y;
	for (count_x = 0; count_x <  MATRIX_SIZE; count_x++)
	{
		printf("\nrow %d: ", count_x);
		for (count_y = 0; count_y < MATRIX_SIZE; count_y++)
			printf(" %d ", matrix[count_x][count_y]);
	}
	printf("\n");
}

void matrix_free(int **matrix, int row_size)
{
	int count_x;
    	for (int count_x = 0; count_x < row_size; count_x++)
    	{
        	free(matrix[count_x]);
    	}
    	free(matrix);
}

int vector_math(int **vector_A, int **vector_B, int size)
{
	int result = 0;
	
	return result;
}

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

int main ()
{
	int rank, size;
	int **matrix_A, **matrix_B, **result;
	
	matrix_init(&matrix_A, MATRIX_SIZE, 1);
	matrix_init(&matrix_B, MATRIX_SIZE, 1);

	matrix_multi(matrix_A, matrix_B, &result, MATRIX_SIZE);

	printf("\nMatrix A:");
	matrix_debug(matrix_A);
	printf("\nMatrix B:");
	matrix_debug(matrix_B);
	printf("\nResult:");
	matrix_debug(result);

	matrix_free(matrix_A, MATRIX_SIZE);
	matrix_free(matrix_B, MATRIX_SIZE);
	matrix_free(result, MATRIX_SIZE);

	printf("\nDONE!");

	return 0;
}
