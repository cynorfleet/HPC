/**************************************************************************
 * FILE: mpi_2dfft.c
 * OTHER FILES: mpi_2dfft.h
 * DESCRIPTION: 
 *   MPI - Two-Dimensional Fast Fourier Transform - C Version
 *   The image originates on a single processor (SOURCE_PROCESSOR).
 *   This image, a[], is distributed by rows to all other processors.  Each
 *   processor then performs a one-dimensional FFT on the rows of the image
 *   stored locally.  The image is then transposed using the MPI_Alltoall()
 *   routine; this partitions the intermediate image by columns.  Each
 *   processor then performs a one-dimensional FFT on the columns of the
 *   image.  Finally, the columns of the image are collected back at the
 *   destination processor and the output image is tested for correctness.
 *   Input is a 512x512 complex matrix. The input matrix is initialized with 
 *   a point source. Output is a 512x512 complex matrix that overwrites 
 *   the input matrix.  Timing and Mflop results are displayed following 
 *   execution. A straightforward unsophisticated 1D FFT kernel is used.  
 *   It is sufficient to convey the general idea, but be aware that there are
 *   better 1D FFTs available on many systems.
 * AUTHOR: George Gusciora
 * LAST REVISED:  04/02/05 Blaise Barney
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <mpi.h>
#include "mpi_2dfft.h"

#define IMAGE_SIZE		512
#define NUM_CELLS		4
#define IMAGE_SLICE		(IMAGE_SIZE / NUM_CELLS)
#define SOURCE_PROCESSOR	0
#define DEST_PROCESSOR		SOURCE_PROCESSOR

int     numtasks;		        /* Number of processors */
int     taskid;			        /* ID number for each processor */
mycomplex a[IMAGE_SIZE][IMAGE_SIZE];	/* input matrix: complex numbers */
mycomplex a_slice[IMAGE_SLICE][IMAGE_SIZE];	
mycomplex a_chunks[NUM_CELLS][IMAGE_SLICE][IMAGE_SLICE];
mycomplex b[IMAGE_SIZE][IMAGE_SIZE];	/* intermediate matrix */
mycomplex b_slice[IMAGE_SIZE][IMAGE_SLICE];
mycomplex b_chunks[NUM_CELLS][IMAGE_SLICE][IMAGE_SLICE];
mycomplex *collect;
mycomplex w_common[IMAGE_SIZE/2];  /* twiddle factors */
struct timeval	etime[10];
int		checkpoint;
float		dt[10], sum;

int main(argc,argv)
int argc;
char *argv[];
{
   int rc, cell, i, j, n, nx, logn, errors, sign, flops;
   float mflops;

   checkpoint=0;

   /* Initialize MPI environment and get task's ID and number of tasks 
    * in the partition. */
   rc = MPI_Init(&argc,&argv);
   rc|= MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
   rc|= MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

   /* Must have 4 tasks for this program */
   if (numtasks != NUM_CELLS)
   {
      printf("Error: this program requires %d MPI tasks\n", NUM_CELLS);
      exit(1);
   }

   if (rc != MPI_SUCCESS)
      printf ("error initializing MPI and obtaining task ID information\n");
   else
      printf ("MPI task ID = %d\n", taskid);

   n = IMAGE_SIZE;
   /* compute logn and ensure that n is a power of two */
   nx = n;
   logn = 0;
   while(( nx >>= 1) > 0)
      logn++;
   nx = 1;
   for (i=0; i<logn; i++)
      nx = nx*2;
   if (nx != n)
   {
      (void)fprintf(stderr, "%d: fft size must be a power of 2\n", IMAGE_SIZE);
      exit(0);
   }

   if (taskid == SOURCE_PROCESSOR)
   {
      for (i=0; i<n; i++)
         for (j=0; j<n; j++)
            a[i][j].r = a[i][j].i = 0.0;
      a[n/2][n/2].r =  a[n/2][n/2].i = (float)n; 
   
      /* print table headings in anticipation of timing results */ 
      printf("512 x 512 2D FFT\n");
      printf("                                 Timings(secs)\n");
      printf("          scatter   1D-FFT-row  transpose 1D-FFT-col  gather");
      printf("        total\n");
   }

   /* precompute the complex constants (twiddle factors) for the 1D FFTs */

   for (i=0;i<n/2;i++)
   {
      w_common[i].r = (float) cos((double)((2.0*PI*i)/(float)n));
      w_common[i].i = (float) -sin((double)((2.0*PI*i)/(float)n));
   }
/*****************************************************************************/
/*                      Distribute Input Matrix By Rows                      */
/*****************************************************************************/
   rc = MPI_Barrier(MPI_COMM_WORLD);
   if (rc != MPI_SUCCESS)
   {
      printf("Error: MPI_Barrier() failed with return code %d\n", rc);
      return(-1);
   }

   gettimeofday(&etime[checkpoint++], (struct timeval*)0);
   rc = MPI_Scatter((char *) a, IMAGE_SLICE * IMAGE_SIZE * 2, MPI_FLOAT,
                    (char *) a_slice, IMAGE_SLICE * IMAGE_SIZE * 2, MPI_FLOAT,
                    SOURCE_PROCESSOR, MPI_COMM_WORLD);
   if (rc != MPI_SUCCESS)
   {
      printf("Error: MPI_Scatter() failed with return code %d\n", rc);
      return(-1);
   }
   gettimeofday(&etime[checkpoint++], (struct timeval*)0);

/*****************************************************************************/
/*                      Perform 1-D Row FFTs                                 */
/*****************************************************************************/
   for (i=0;i<IMAGE_SLICE;i++)
      fft(&a_slice[i][0], w_common, n, logn);
   gettimeofday(&etime[checkpoint++], (struct timeval*)0);
/*****************************************************************************/
/*			Transpose 2-D image				     */
/*****************************************************************************/
   for(cell=0;cell<NUM_CELLS;cell++)
   {
      for(i=0;i<IMAGE_SLICE;i++)
      {
         for(j=0;j<IMAGE_SLICE;j++)
         {
            a_chunks[cell][i][j].r =
               a_slice[i][j + (IMAGE_SLICE * cell)].r;
            a_chunks[cell][i][j].i =
               a_slice[i][j + (IMAGE_SLICE * cell)].i;
         }
      }
   }

   rc = MPI_Alltoall(a_chunks, IMAGE_SLICE * IMAGE_SLICE * 2, MPI_FLOAT,
                     b_slice, IMAGE_SLICE * IMAGE_SLICE * 2, MPI_FLOAT,
                     MPI_COMM_WORLD);
   if (rc != MPI_SUCCESS)
   {
      printf("Error: MPI_Alltoall() failed in cell %d return code %d\n", taskid, rc);
      return(-1);
   }
   gettimeofday(&etime[checkpoint++], (struct timeval*)0);
/*****************************************************************************/
/*                      Perform 1-D Column FFTs                              */
/*****************************************************************************/
   for(i=0;i<IMAGE_SLICE;i++)
   {
      for(j=0;j<IMAGE_SIZE;j++)
      {
         a_slice[i][j].r = b_slice[j][i].r;
         a_slice[i][j].i = b_slice[j][i].i;
      }
   }
   for (i=0;i<IMAGE_SLICE;i++)
      fft(&a_slice[i][0], w_common, IMAGE_SIZE, logn);
   gettimeofday(&etime[checkpoint++], (struct timeval*)0);
/*****************************************************************************/
/*                    Undistribute Output Matrix by Rows                     */
/*****************************************************************************/
   collect = ( mycomplex *) malloc(IMAGE_SIZE * IMAGE_SIZE * sizeof( mycomplex));
   rc = MPI_Gather(a_slice, IMAGE_SLICE * IMAGE_SIZE * 2, MPI_FLOAT,
                   a, IMAGE_SLICE * IMAGE_SIZE * 2, MPI_FLOAT,
                   DEST_PROCESSOR, MPI_COMM_WORLD);
   if (rc != MPI_SUCCESS)
   {
      printf("Error: MPI_Gather() failed with return code %d\n", rc);
      fflush(stdout);
   }

   if (taskid == DEST_PROCESSOR)
   {
      for(i=0;i<IMAGE_SIZE;i++)
      {
         for(j=0;j<IMAGE_SIZE;j++)
         {
            b[i][j].r = a[j][i].r;
            b[i][j].i = a[j][i].i;
         }
      }
   }

   gettimeofday(&etime[checkpoint++], (struct timeval*)0);
   fflush(stdout);

   /* Calculate event timings and flops -  then print them */
   for(i=1;i<checkpoint;i++)
      dt[i] = ((float) ((etime[i].tv_sec - etime[i-1].tv_sec) * 1000000 +
                         etime[i].tv_usec - etime[i-1].tv_usec)) / 1000000.0;
   printf("cell %d:   ", taskid);
   for(i=1;i<checkpoint;i++)
      printf("%2.6f   ", dt[i]);
   sum=0;
   for(i=1;i<checkpoint;i++)
      sum+=dt[i];
   printf("  %2.6f \n", sum);

   if (taskid == DEST_PROCESSOR)
   {
      flops = (n*n*logn)*10; 
      mflops = ((float)flops/1000000.0);
      mflops = mflops/(float)sum;
      printf("Total Mflops= %3.4f\n", mflops);

      errors = 0;
      for (i=0;i<n;i++)
      {
         if (((i+1)/2)*2 == i)
            sign = 1;
         else
            sign = -1;
         for (j=0;j<n;j++)
         {
            if (b[i][j].r > n*sign+EPSILON ||
                b[i][j].r < n*sign-EPSILON ||
                b[i][j].i > n*sign+EPSILON ||
                b[i][j].i < n*sign-EPSILON)
            {
               printf("[%d][%d] is %f,%f should be %f\n", i, j,
                                     b[i][j].r, b[i][j].i, (float) n*sign);
               errors++;
            }
            sign *= -1;
         }
      }
      if (errors)
      {
         printf("%d errors!!!!!\n", errors);
         exit(0);
      }
   }
/**********
   if (taskid == DEST_PROCESSOR)
   {
      for(i=0;i<IMAGE_SIZE;i++)
      {
         printf("Row %d\n",i);
         for(j=0;j<IMAGE_SIZE;j++)
            printf("%f %f\n", a[i][j].r, a[i][j].i);
      }
   }
**********/


   MPI_Finalize();
   exit(0);
}

fft(data,w_common,n,logn)
mycomplex *data,*w_common;
int n,logn;
{
  int incrvec, i0, i1, i2, nx;
  float f0, f1;
  void bit_reverse();

  /* bit-reverse the input vector */
  (void)bit_reverse(data,n);

  /* do the first logn-1 stages of the fft */
  i2 = logn;
  for (incrvec=2;incrvec<n;incrvec<<=1) {
    i2--;
    for (i0 = 0; i0 < incrvec >> 1; i0++) {
      for (i1 = 0; i1 < n; i1 += incrvec) {
        f0 = data[i0+i1 + incrvec/2].r * w_common[i0<<i2].r - 
	  data[i0+i1 + incrvec/2].i * w_common[i0<<i2].i;
        f1 = data[i0+i1 + incrvec/2].r * w_common[i0<<i2].i + 
	  data[i0+i1 + incrvec/2].i * w_common[i0<<i2].r;
        data[i0+i1 + incrvec/2].r = data[i0+i1].r - f0;
        data[i0+i1 + incrvec/2].i = data[i0+i1].i - f1;
        data[i0+i1].r = data[i0+i1].r + f0;
        data[i0+i1].i = data[i0+i1].i + f1;
      }
    }
  }

  /* do the last stage of the fft */
  for (i0 = 0; i0 < n/2; i0++) {
    f0 = data[i0 + n/2].r * w_common[i0].r - 
      data[i0 + n/2].i * w_common[i0].i;
    f1 = data[i0 + n/2].r * w_common[i0].i + 
      data[i0 + n/2].i * w_common[i0].r;
    data[i0 + n/2].r = data[i0].r - f0;
    data[i0 + n/2].i = data[i0].i - f1;
    data[i0].r = data[i0].r + f0;
    data[i0].i = data[i0].i + f1;
  }
}

/* 
 * bit_reverse - simple (but somewhat inefficient) bit reverse 
 */
void bit_reverse(a,n)
mycomplex *a;
int n;
{
  int i,j,k;

  j = 0;
  for (i=0; i<n-2; i++){
    if (i < j)
      SWAP(a[j],a[i]);
    k = n>>1;
    while (k <= j) {
      j -= k; 
      k >>= 1;
    }
    j += k;
  }
}
