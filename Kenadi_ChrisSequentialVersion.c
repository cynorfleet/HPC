//Assignment #3
//Name: Kenadi Campbell, and Christian Norfleet
//Parallel Programming Date: 3/31/2017
//*****************************************************
//*****************************************************
/*In this program, we implemented the radix-2 approach
of the FFT algorithm. It uses something similar to the
way you would approach the problem by hand. Splitting
the data set up into odd and even sections, and then
find their total's seperately, and then adding them
together to get the total of the entire set.

Compile: gcc Kenadi_ChristSequentialVersion.c -o FFT.exe -lm
Execute: ./FFT.exe*/
//*****************************************************
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include "timer.h"
#include <mpi.h>

#define N 16384//the number of values to be printed
#define PI 3.14159265
#define MASTER 0

int Partition(int numprocessors, int samplesize);
int main(int argc, char**argv)
{
	int i;
	for(i = 0; i<3; i++)
	{
		//variables
		double start, finish;
		double elapsedTime;
		//start clock
		GET_TIME(start);
		double complex sample[N];
		double result[N];
		int i = 0;
		int j =0;
		int K=0;
		int k=0;
		int x=0;
		int n=0;
		double a,b,c,d;
		double total=0;
	  int rank = 0;
	  int size = 0;
		int partition = 0;

		//open input/output files
		FILE *infile, *outfile;
		infile=fopen("infile.txt","r");
		outfile=fopen("outfile.txt","w");

   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD,&size);
   MPI_Comm_rank(MPI_COMM_WORLD,&rank);
   partition = Partition(size, N);
		//start timing
		//start = clock();
		//loop through array and fill with given data
		if (rank == 0){
		for(i=0; i<8; i++)
		{
			//readin from file
			double real, imagine;
			fscanf(infile, "%lf %lf\n", &real, &imagine);
			sample[i] = real + imagine * I;
		}
		//fill remaining spaces of array with zeros
		for(i=8; i<N; i++)
		{
			sample[i] = 0 + 0*I;
		}
		//more variables
		double a1=0;
		double a2=0;
		double b1=0;
		double b2=0;
		double complex evenresult=0;
		double complex oddresult=0;
		double complex totalresult=0;
		double complex totalevenresult = 0;
		double complex totaloddresult = 0;

		for(K=0; K<N; K++)
		{
			//resest to zero after each X(K)
			totalevenresult = 0;
			totaloddresult = 0;
			//arithemtic
			for(n=0; n<4; n++)
			{
				//if even sample
				if((2*n) % 2 == 0 || 2*n == 0)
				{
					//the values from the x(2n) section of the algorithm
					a=creal(sample[2*n]);
					b=cimag(sample[2*n]);
					double complex a1 = a+b*I;

					//the values from the e^-(i(2*PI*2*n*K)/N) section of algorithm
					c = cos((2*PI*(2*n)*K)/N);
					d = sin((2*PI*(2*n)*K)/N);
					double complex a2 = c-d*I;
					evenresult = (a1 * a2);
					//compute total result of even sections
					totalevenresult += evenresult;
				}
				//if odd sample
				if(((2*n)+1) % 2 == 1)
				{
					a=creal(sample[(2*n)+1]);
					b=cimag(sample[(2*n)+1]);
					double complex b1 = a+b*I;

					c = cos((2*PI*((2*n)+1)*K)/N);
					d = sin((2*PI*((2*n)+1)*K)/N);
					double complex b2 = c-d*I;
					oddresult = (b1 * b2);
					//compute total result of odd sections
					totaloddresult += oddresult;
				}
			}
			//computes total result, both even and odd put together
			totalresult = totalevenresult + totaloddresult;
			//stop timing, compute time passed
			GET_TIME(finish);
			elapsedTime = finish - start;
			//print the total
			if(K<10)
				printf("X(%d) = %.1lf + %.1lfi\n",K, creal(totalresult), cimag(totalresult));
		}
	}
	MPI_Finalize();
		//print the time elapsed
		printf("Time: %lf seconds\n\n", elapsedTime);
	}
		return 0;
}

int Partition(int numprocessors, int samplesize)
{
  return (samplesize/numprocessors);
}
