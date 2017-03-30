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

#define K 1024
#define N 8
#define PI 3.14159265

int main()
{
  // variables
  double sample[K][2];
  double result[K];
  int i = 0;
  int j =0;
  int k=0;
  int x=0;
  double a,b,c,d;
  double total=0;
  //read from file
  FILE *fptr;
  fptr=fopen("infile.txt","r");

  for(i=0; i<N; i++)
  {
      double temp;
      for(j=0; j<2; j++)
        {
          fscanf(fptr, "%lf", &temp);
          sample[i][j] = (temp);
        }
    }
    for(i=N+1; i<K; i++)
    {
      for(j=0; j<2; j++)
        {
          sample[i][j] = 0;
        }
    }

    double a1=0;
    double a2=0;
    double b1=0;
    double b2=0;
    double evenresult=0;
    double oddresult=0;
    double totalresult=0;

  for(x=0; x<K; x++)
  {
    //EvenOrOdd
    for(i=0; i<N; i++)
    {
        // if even sample
        if(i % 2 == 0 || i == 0)
        {
          for(j=0; j<1; j++)
          {
            a=sample[i][j];
            b=sample[i][j+1];
            double complex a1 = a+b*I;
          }
          c = cos((2*PI*i)/N);
          d = sin((2*PI*i)/N);
          double complex a2 = c+d*I;
          printf("even: %.1lf", evenresult);
          double complex evenresult = evenresult + (a1 * a2);
        }
        // if odd sample
        else if (i % 2 == 1)
        {
          for(j=0; j<1; j++)
          {
            a=sample[i][j];
            b=sample[i][j+1];
            double complex b1 = a+b*I;
          }
          c = cos((2*PI*i)/N);
          d = sin((2*PI*i)/N);
          double complex b2 = c+d*I;
          double complex oddresult = oddresult + (b1 * b2);
          printf("odd: %.1lf ", cimag(oddresult));
        }
      }
    // compute total by adding results
    for(i=0; i<N; i++)
    {
      double complex totalresult = evenresult+oddresult;
    }
  }
  printf("X(%d) = %lf",i, totalresult);
  return 0;
}
