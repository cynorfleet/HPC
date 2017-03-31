//			Christian and Johnny
//		Serial Matrix Multiplication
//		Program will multiply 2 square matricies
//		and return result to a 3rd matrix.
//		User must set the MATRIX_SIZE variable
//
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <sys/time.h>
#include <time.h>
#include <mpi.h>

#define MASTER 0
#define N 8
#define PI 3.14159265

int main(int argc, char **argv)
{
  // variables
  double complex sample[N];
  double result[N];
  int numprocessors = 0;
  int rank = 0;
  int size = 0;
  int i = 0;
  int j =0;
  int K=0;
  int k=0;
  int x=0;
  int n=0;
  int partition = 0;
  double a,b,c,d;
  double total=0;

  //read from file
  FILE *infile, *outfile;
  infile=fopen("infile.txt","r");
  outfile=fopen("outfile.txt","w");

   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD,&size);
   MPI_Comm_rank(MPI_COMM_WORLD,&rank);

   partition = Partition(size, N);

// initialize values
if (rank == MASTER)
{
  for(i=0; i<N; i++)
  {
      //readin from file
       double real, imagine;
       if (fscanf(infile, "%lf %lf\n", &real, &imagine))
        sample[i] = real + imagine * I;
       else
        printf("could not read file");
        
       fprintf(outfile, "%.1lf %.1fi\n", creal(sample[i]), cimag(sample[i]));

  }
    for(i=N+1; i<K; i++)
    {
       sample[i] = 0 + 0*I;
       printf("Zero %.1lf %.1fi\n", creal(sample[i]), cimag(sample[i]));
    }
}
  // sync the processes
  MPI_Barrier(MPI_COMM_WORLD);
  // send the data for calculation
  MPI_Scatter(sample, partition, MPI_C_DOUBLE_COMPLEX, sample, partition, MPI_C_DOUBLE_COMPLEX, MASTER, MPI_COMM_WORLD);

    double a1=0;
    double a2=0;
    double b1=0;
    double b2=0;
    double complex evenresult=0;
    double complex oddresult=0;
    double complex totalresult=0 + 0*I;
    double complex totalodd = 0 + 0*I;
    double complex totaleven = 0 + 0*I;

  for(K=0; K<N; K++)
  {
    //EvenOrOdd
    for(n=0; n<4; n++)
    {
        // if even sample
        if((2*n) % 2 == 0 || 2*n == 0)
        {
            a=creal(sample[2*n]);
            b=cimag(sample[2*n]);
            double complex a1 = a+b*I;
            printf("even a1: %.1lf + %.1fi\n", creal(a1), cimag(a1));
            // fprintf(outfile,"even a1: %.1lf + %.1fi\n", creal(a1), cimag(a1));

          c = cos((2*PI*(2*n)*K)/N);
          d = sin((2*PI*(2*n)*K)/N);
          double complex a2 = c-d*I;
            printf("even a2: %.1lf + %.1fi\n", creal(a2), cimag(a2));
            // fprintf(outfile,"even a2: %.1lf + %.1fi\n", creal(a2), cimag(a2));
          evenresult = (a1 * a2);
          printf("even result: %.1lf + %.1fi\n", creal(evenresult), cimag(evenresult));
          // fprintf(outfile,"even result: %.1lf + %.1fi\n", creal(evenresult), cimag(evenresult));
        }
        // if odd sample
        if(((2*n)+1) % 2 == 1)
        {
            a=creal(sample[(2*n)+1]);
            b=cimag(sample[(2*n)+1]);
            double complex b1 = a+b*I;
            printf("odd b1: %.1lf + %.1fi\n", creal(b1), cimag(b1));

          c = cos((2*PI*((2*n)+1)*K)/N);
          d = sin((2*PI*((2*n)+1)*K)/N);
          double complex b2 = c-d*I;
            printf("odd b2: %.1lf + %.1fi\n", creal(b2), cimag(b2));
          oddresult = (b1 * b2);
          printf("odd result: %.1lf + %.1fi\n", creal(oddresult), cimag(oddresult));
        }
        totaleven += creal(evenresult) + cimag(evenresult);
        totalodd += oddresult;
        printf("totaleven: %.1lf + %.1lf\n", creal(totaleven), cimag(totaleven));
        printf("totalodd: %.1lf + %.1lf\n", creal(totalodd), cimag(totalodd));
      }

   MPI_Gather(sample, partition, MPI_C_DOUBLE_COMPLEX, sample, partition, MPI_C_DOUBLE_COMPLEX, MASTER, MPI_COMM_WORLD);
if(rank == MASTER)
{
      printf("DONE!!!!\n");
    printf("X(%d) = %.1lf + %.1lf\n\n",K, creal(totalresult), cimag(totalresult));
}
  }
  //fprintf(outfile, "X(%d) = %.1lf\n\n",i, creal(totalresult));
  MPI_Finalize();
  return 0;
}

int Partition(int numprocessors, int samplesize)
{
  return (samplesize/numprocessors);
}
