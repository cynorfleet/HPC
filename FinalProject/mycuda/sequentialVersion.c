//******************************************************************************
// HPC Hm3
// Name: Andrew McKissick and Benjamin Shelton
// Parallel Programming Date: 3/31/17
//******************************************************************************
// This program performs a Fast Fourier transform on a set of complex numbers.
// Input is read from a file (data.txt) as floating point pairs. Error checking
// is not performed on input data, so anything other than an even number less
// than or equal to 2*N of decimal numbers separated by whitespace will cause
// undefined behavior. Output is to stdout and limited to the first 11 terms,
// and the time required for program and funcion execution, though this is
// easily configurable in the code. No user interaction is required beyond
// creating a valid input file and running the program.
//******************************************************************************

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>

//number of elements in each array
#define N 1024

void FFTCalc(double realIn[], double imagIn[], double realOut[], double imagOut[]);

int main()
{
	clock_t calc_start, calc_end;
	double calc_time;
	
	FILE *data;
	data = fopen("data.txt", "r");

	double xReal[N];
	double xImag[N];

	//read from data.txt into the real and imaginary arrays until EOF
	int i = 0;
	while (fscanf(data, "%lf", &xReal[i]) != EOF)
	{
		fscanf(data, "%lf", &xImag[i]);
		i++;
	}

	fclose(data);

	//fill the remaining indices in the real and imaginary arrays with 0.0
	for (; i < N; i++)
	{
		xReal[i] = 0.0;
		xImag[i] = 0.0;
	}

	double XReal[N];
	double XImag[N];
	
	//calculate the FFT
	calc_start = clock();
	FFTCalc(xReal, xImag, XReal, XImag);
	calc_end = clock();
	
	calc_time = (double)(calc_end - calc_start) / CLOCKS_PER_SEC;

	//display the first 10 values
	printf("TOTAL PROCESSED SAMPLES: %d\n", N);
	printf("================================================================================\n");
	
	for (i = 0; i < 11; i++)
	{
		printf("XR[%d]: %f\n", i, XReal[i]);
		printf("XI[%d]: %f\n\n", i, XImag[i]);
	}
	
	printf("================================================================================\n\n");
	printf("Time required for calculation: %f\n", calc_time);
	
	return 0;
}

//******************************************************************************
// FFTCalc()
//Parameters: double realIn[], double imagIn[], double realOut[], double imagOut[]
// This function calculates the individual coefficients of the fourier
// transform. It requires two double arrays of size N containing the real and
// imaginary parts of the function to be transformed, along with two double
// arrays for storing the real and imaginary parts of the result. No error check
// is performed so if these requirements are not met it is likely the program
// will crash. This function has been modified from its original version to
// calculate the entire transform rather than individual indices, which should
// increase efficiency by avoiding unnecessary context switches and variable
// declarations.
//******************************************************************************
void FFTCalc(double realIn[], double imagIn[], double realOut[], double imagOut[])
{
	//accumulators. e = even index, o = odd index.
	double eReal, eImag, oCosReal, oCosImag, oSinReal, oSinImag;
	
	int k = 0;

	for(; k < N; k++)
	{
		//frequently used elements. Precalculated here or at the beginning of the inner loop
		double theta = (-2.0 * M_PI * (double)k) / (double)N;
		double cos2MTheta = 0, sin2MTheta = 0;
		int m = 0, twoM = 0, twoMPlus = 0;

		eReal = 0; eImag = 0; oCosReal = 0; oCosImag = 0; oSinReal = 0; oSinImag = 0;
		for (; m < (N / 2); m++)
		{
			twoM = 2 * m;
			twoMPlus = twoM + 1;
			cos2MTheta = cos((double)twoM * theta);
			sin2MTheta = sin((double)twoM * theta);

			eReal += ((realIn[twoM] * cos2MTheta) - (imagIn[twoM] * sin2MTheta));
			eImag += ((realIn[twoM] * sin2MTheta) + (imagIn[twoM] * cos2MTheta));
			oCosReal += ((realIn[twoMPlus] * cos2MTheta) - (imagIn[twoMPlus] * sin2MTheta));
			oCosImag += ((realIn[twoMPlus] * sin2MTheta) + (imagIn[twoMPlus] * cos2MTheta));
			oSinReal -= ((realIn[twoMPlus] * sin2MTheta) + (imagIn[twoMPlus] * cos2MTheta));
			oSinImag += ((realIn[twoMPlus] * cos2MTheta) - (imagIn[twoMPlus] * sin2MTheta));
		}
		realOut[k] = eReal + cos(theta) * oCosReal + sin(theta) * oSinReal;
		imagOut[k] = eImag + cos(theta) * oCosImag + sin(theta) * oSinImag;
	}

	return;
}