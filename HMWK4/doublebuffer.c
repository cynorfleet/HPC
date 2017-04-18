
//**************************************************************
// Assignment #4
// Name: Christian Norfleet
// Parallel Programming Date: Date of Submission (4/17/2017)
//***************************************************************
// This program will implement the use of the double buffering technique.
// It will utilize a total of 2 nodes @ 1 process per node.
// Process 1 in node A, will have an array of 100 million numbers,
// each one of the numbers will be initialized to zero. This process will
// send packages of 100000 numbers to the process running in the second node.
// In the process running on the second node you will implement the double
// buffer technique. The process at the second node will receive the info in
// the second half of the buffer while it is also processing data in the other half.
//*****************************************************************

#include <stdlib.h>
#include <stdio.h>
#include </usr/include/mpi/mpi.h>
#define SIZE_STREAM 100000000
#define SIZE_BUFFER (SIZE_STREAM*2)
#define SIZE_BLOCK 100000
#define STREAMER 0
#define BUFFER 1
//*******************************************************************
// Function Name::main()
// Parameters: int argc, char *argv[]
//              command line arguments to be sent to Program
int main(int argc, char *argv[])
{
  int rank, size;
  // standard MPI initialization boilerplate
  MPI_Request request;
  MPI_Status status;
  MPI_Init(&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);

  if(rank == 0)
  {
    int NodeA[SIZE_STREAM] = {0};
    // starting location of stream
    int block_loc = 0;
    // loop until all blocks are sent
    while(NodeA + block_loc < NodeA+SIZE_STREAM)
    {
      // Send a block of data to NodeB
      MPI_Isend(NodeA + block_loc, SIZE_BLOCK, MPI_INT, BUFFER, 0, MPI_COMM_WORLD, &request);
      // keep track of current location in stream
      block_loc = block_loc + SIZE_BLOCK;
    }
  }

  if(rank == 1)
  {
    int i, z;
    int NodeB[SIZE_BUFFER];
    int buffer_loc = SIZE_STREAM;
    int compute_loc = 0;
    // while not @ end of buffer
    while(buffer_loc < SIZE_BUFFER)
    {
      // if there is work to be done do it
      // the next value would have to be less than or equal
      // to previous value for work to be done
      if(NodeB[compute_loc] > -1 && NodeB[compute_loc] >= NodeB[compute_loc] + 1)
      {
      // Recieve data in the second half
      MPI_Irecv(NodeB+buffer_loc, SIZE_BLOCK, MPI_INT, STREAMER, 0, MPI_COMM_WORLD, &request);
      // loop thru entire block
      for(i = 1; i < SIZE_BLOCK; i ++)
        {
          // and 1 and store in main buffer
          NodeB[compute_loc] = NodeB[buffer_loc] + 1;
          // multiply by 1 two times
          for(z = 0; z < 2; z++)
            NodeB[compute_loc] = NodeB[compute_loc] * 1;
          printf("value: %d\n", NodeB[compute_loc]);
          // keep track of current location in buffer
          compute_loc = compute_loc + 1;
          buffer_loc = buffer_loc + 1;
        }
      }
    }
  }
  MPI_Finalize();
  return 0;
}
