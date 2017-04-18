//******************************************************************************
// Final Project
// Name: Andrew McKissick
// GPU Computing Date: 12/5/16
//******************************************************************************
// This program performs a Fast Fourier transform on a set of complex numbers.
// Input is read from a file (data.txt) as floating point pairs. Error checking
// is not performed on input data, so anything other than an even number less
// than or equal to 2*N of decimal numbers separated by whitespace will cause
// undefined behavior. Output is to stdout and limited to the first 8 terms,
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
#define N 16384

__global__ void FFTCalc(double realIn[], double imagIn[], double realOut[], double imagOut[]);

int main()
{
	//clock_t prog_start, prog_end, calc_start, calc_end, read_start, read_end;
	float comm_time_1, comm_time_2, calc_time;
	cudaEvent_t start, stop;
	
	//prog_start = clock();
	FILE *data;
	data = fopen("data.txt", "r");
	
	const int DATASIZE = N*sizeof(double);

	double xReal[N];
	double xImag[N];
	
	double* dev_xReal;
	double* dev_xImag;
	
	double* dev_XReal;
	double* dev_XImag;

	//read from data.txt into the real and imaginary arrays until EOF
	//read_start = clock();
	int i = 0;
	while (fscanf(data, "%lf", &xReal[i]) != EOF)
	{
		fscanf(data, "%lf", &xImag[i]);
		i++;
	}
	//read_end = clock();

	fclose(data);

	//fill the remaining indices in the real and imaginary arrays with 0.0
	for (; i < N; i++)
	{
		xReal[i] = 0.0;
		xImag[i] = 0.0;
	}
	
	cudaEventCreate(&start);
  cudaEventRecord(start,0);
	
	cudaMalloc((void**)&dev_xReal, DATASIZE);
	cudaMalloc((void**)&dev_xImag, DATASIZE);
	
	cudaMalloc((void**)&dev_XReal, DATASIZE);
	cudaMalloc((void**)&dev_XImag, DATASIZE);
	
	cudaMemcpy(dev_xReal, xReal, DATASIZE, cudaMemcpyHostToDevice);
	cudaMemcpy(dev_xImag, xImag, DATASIZE, cudaMemcpyHostToDevice);
	
	cudaEventCreate(&stop);
	cudaEventRecord(stop,0);
	cudaEventSynchronize(stop);
	
	cudaEventElapsedTime(&comm_time_1, start, stop);
	
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
	
	dim3 dimGrid(N/1024, 1); 	
	dim3 dimBlock(1024, 1);

	double XReal[N];
	double XImag[N];

	//calculate the FFT
	//calc_start = clock();
	
	cudaEventCreate(&start);
  cudaEventRecord(start,0);
	
	FFTCalc<<<dimGrid, dimBlock>>>(dev_xReal, dev_xImag, dev_XReal, dev_XImag);
	cudaThreadSynchronize();
	
	cudaEventCreate(&stop);
	cudaEventRecord(stop,0);
	cudaEventSynchronize(stop);
	
	cudaEventElapsedTime(&calc_time, start, stop);
	
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
	
	//calc_end = clock();
	
	cudaEventCreate(&start);
  cudaEventRecord(start,0);
	
	cudaMemcpy(XReal, dev_XReal, DATASIZE, cudaMemcpyDeviceToHost);
	cudaMemcpy(XImag, dev_XImag, DATASIZE, cudaMemcpyDeviceToHost);
	
	cudaFree(dev_xReal);
	cudaFree(dev_xImag);
	
	cudaFree(dev_XReal);
	cudaFree(dev_XImag);
	
	cudaEventCreate(&stop);
	cudaEventRecord(stop,0);
	cudaEventSynchronize(stop);
	
	cudaEventElapsedTime(&comm_time_2, start, stop);
	
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
	
	//prog_end = clock();
	
	//prog_time = (double)(prog_end - prog_start) / CLOCKS_PER_SEC;
	//calc_time = (double)(calc_end - calc_start) / CLOCKS_PER_SEC;
	//read_time = (double)(read_end - read_start) / CLOCKS_PER_SEC;

	//display the first 8 values
	printf("TOTAL PROCESSED SAMPLES: %d\n", N);
	printf("================================================================================\n");
	for (i = 0; i < 8; i++)
	{
		printf("XR[%d]: %f\n", i, XReal[i]);
		printf("XI[%d]: %f\n\n", i, XImag[i]);
	}
	printf("================================================================================\n\n");
	printf("Time required for calculation: %f\n", calc_time);
	printf("Time required for communication: %f\n", (comm_time_1 + comm_time_2));

	//printf("Time required for file read: %f\n", read_time);
	//printf("Time required for entire program: %f\n", prog_time);

	return 0;
}

//******************************************************************************
// FFTCalc()
// This kernel calculates the coefficients of the fourier transform. It requires
// two double arrays of size N containing the real and imaginary parts of the
// function to be transformed, and two double arrays for storing the real and
// imaginary parts of the result. No error check is performed so if these
// requirements are not met it is likely the program will crash. This program
// computes large sums in sequence to avoid data corrumption. While it is
// possible to make these calculations more parallel, I felt that given the
// number of data accesses this would require and the number of threads
// available on a node, that this would likely be counterproductive to
// efficiently calculating the fourier transform.
//******************************************************************************
__global__ void FFTCalc(double realIn[], double imagIn[], double realOut[], double imagOut[])
{
	//accumulators. e = even index, o = odd index.
	double eReal = 0, eImag = 0, oCosReal = 0, oCosImag = 0, oSinReal = 0, oSinImag = 0;

	//frequently used elements. Precalculated here or at the beginning of the loop
	double theta = (-2.0 * M_PI * (double)(threadIdx.x + blockIdx.x * 1024)) / (double)N;
	double cos2MTheta = 0, sin2MTheta = 0;
	int m = 0, twoM = 0, twoMPlus = 0;

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
	realOut[threadIdx.x + blockIdx.x * 1024] = eReal + cos(theta) * oCosReal + sin(theta) * oSinReal;
	imagOut[threadIdx.x + blockIdx.x * 1024] = eImag + cos(theta) * oCosImag + sin(theta) * oSinImag;

	return;
}