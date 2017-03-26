/* Dijkstra's Algorithm in C */
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<time.h>

#define IN 99
#define N 6

int dijkstra(int cost[][N], int source, int target);
void strrev(char str[100]);
void header(FILE *outfile);
int main(int argc, char *argv[])
{
    //openFiles
    FILE *infile, *outfile;
    infile=fopen("infile.txt", "r");
    outfile=fopen("outfile.txt", "a");
    header(outfile);

    int node_count = atoi(argv[1]) - 1;
    int edge_count = node_count*(node_count-1)/2;

    char tempnode[100];
    int node, out_counter=1;


    int cost[N][N],i,j,w,ch,co;
    int source, target,x,y;
    char *msg;

    fprintf(outfile, "\t The Shortest Path Algorithm ( DIJKSTRA'S ALGORITHM in C ) \n\n");
    fprintf(outfile, "[node count: %d edge count: %d]\n", node_count, edge_count);

    printf("\t The Shortest Path Algorithm ( DIJKSTRA'S ALGORITHM in C ) \n\n");
    printf("node count: %d edge count: %d\n", node_count, edge_count);

    for(i=1;i< N;i++)
    for(j=1;j< N;j++)
    cost[i][j] = edge_count;
    for(x=1;x< N;x++)
    {
        for(y=x+1;y< N;y++)
        {
            fscanf(infile, "%[^\n]\n", tempnode);
            node = atoi(tempnode);

            fprintf(outfile,"Weight of the path between nodes %d and %d: %d\n",x,y,node);
            printf("Weight of the path between nodes %d and %d: %d\n",x,y,node);
            cost [x][y] = cost[y][x] = node;
        }

        fprintf(outfile, "\n");
        printf("\n");
    }

    fscanf(infile,"%[^\n.*:start]\n", tempnode);
    source = atoi(tempnode);
    fprintf(outfile, "\nSource node: %d\n", source);
    printf("\nSource node: %d\n", source);

    for( out_counter= source; out_counter <= node_count; out_counter++)
    {
        if(out_counter != source)
        {
            co = dijsktra(cost,source,out_counter);
            fprintf(outfile, "\tThe Shortest Path from %d to %d: %d\n",source,out_counter,co);
            printf("\tThe Shortest Path from %d to %d: %d\n",source,out_counter,co);
        }
    }
    fclose(infile);
    fclose(outfile);
}

int dijsktra(int cost[][N],int source,int target)
{
    int dist[N],prev[N],selected[N]={0},i,m,min,start,d,j;
    char path[N];
    for(i=1;i< N;i++)
    {
        dist[i] = IN;
        prev[i] = -1;
    }
    start = source;
    selected[start]=1;
    dist[start] = 0;
    while(selected[target] ==0)
    {
        min = IN;
        m = 0;
        for(i=1;i< N;i++)
        {
            d = dist[start] +cost[start][i];
            if(d< dist[i]&&selected[i]==0)
            {
                dist[i] = d;
                prev[i] = start;
            }
            if(min>dist[i] && selected[i]==0)
            {
                min = dist[i];
                m = i;
            }
        }
        start = m;
        selected[start] = 1;
    }
    start = target;
    j = 0;
    while(start != -1)
    {
        path[j++] = start+65;
        start = prev[start];
    }
    path[j]='\0';
    strrev(path);
    printf("%s", path);
    return dist[target];
}

void strrev(char str[100])
{
    char temp;
    int i, j = 0;

    i = 0;
    j = strlen(str) - 1;

    while (i < j)
    {
       temp = str[i];
       str[i] = str[j];
       str[j] = temp;
       i++;
       j--;
    }
}

void header(FILE *outfile)
{
  time_t curtime;
  struct tm *loc_time;

  //Getting current time of system
  curtime = time (NULL);

  // Converting current time to local time
  loc_time = localtime (&curtime);

char *seperator = "--------------------";
  // Displaying date and time in standard format
  fprintf(outfile, "\n\n%s %s\n", seperator, asctime (loc_time));
}
