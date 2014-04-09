#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define MAX_DATA_SIZE	1000000
#define TAG_A	 100
void RandArray(int *A, int n,  int seed);
void OutPutArray(int *A, int n);
int readFromFile(int **A );

double g_transTime = 0, g_totalTime = 0, g_beginTime=0;

void swap(int *A, int i, int j)
{
    int tmp;
	
    tmp=A[i];
    A[i]=A[j];
    A[j]=tmp;
}

inline int findpivot(int *A, int i, int j)
{
    return (i+j)/2;  
}

int Partition(int *A, int l, int r, int pivot)   
{
    do{
        while(A[++l]<pivot);
        while(A[--r]>pivot);     
        swap(A, l, r);                
    }while(l<r);  
	
    swap(A, l, r);  
	
    return l;               
}

void quick_Sort(int *A, int i,  int j) 
{
    int pivotindex, k;
	
    if(j<=i)            
        return;     
	
    pivotindex=findpivot(A, i, j);
    swap(A, pivotindex, j);                              
    k=Partition(A, i-1, j, A[j]);
    swap(A, k, j);                                                 
	
    quick_Sort(A, i, k-1);
    quick_Sort(A, k+1, j);
}

int QuickSort(int *A, int size, int n, int myid, int p)
{
    int i, j, last, step;
    int *tmpRoot, *tmp;
    int *sendcounts, *recvcounts, *displs, *redispls; 

    printf("Before step1, %d:", myid);
    OutPutArray(A, size);
	
    quick_Sort(A, 0, size-1);

    printf("After step1, %d:", myid);
    OutPutArray(A, size);
	
    if(0==myid)
        tmpRoot=(int*)malloc(sizeof(int)*p*p);
    tmp=(int*)malloc(sizeof(int)*size*2);
    step= n/(p*p);
    for(i=0, j=0; i<p; i++, j+=step)
        tmp[i]=A[j];
    MPI_Gather(tmp, p, MPI_INT, tmpRoot, p, MPI_INT, 0, MPI_COMM_WORLD);
    if(0==myid)
    {
        printf("Before step2, %d:", myid);
        OutPutArray(tmpRoot, p*p);
		
        quick_Sort(tmpRoot, 0, p*p-1);
		
        printf("After step2(1), %d:", myid);
        OutPutArray(tmpRoot, p*p);
		
        for(i=0, j=p+p/2-1; i<p-1; i++, j+=p)
            tmp[i]=tmpRoot[j];
		
        printf("After step2(2), %d:", myid);
        OutPutArray(tmp, p-1);
		
        free(tmpRoot);
    }
    MPI_Bcast(tmp, p-1, MPI_INT, 0, MPI_COMM_WORLD);
	
    printf("After step2(3), %d:", myid);
    OutPutArray(tmp, p-1);
	
    sendcounts=(int*)malloc(sizeof(int)*p);
    recvcounts=(int*)malloc(sizeof(int)*p);
    displs=(int*)malloc(sizeof(int)*p);
    redispls=(int*)malloc(sizeof(int)*p);
	
    last=0;
    displs[0]=0;
    for(i=0, j=0; i<p-1; i++)
    {
        while(j<size && A[j]<tmp[i])
            j++;
        displs[i+1]=j;
        sendcounts[i]=j-last;
        last=j;
    }
    sendcounts[p-1]=size-j;

    printf("Sendcounts, %d:", myid);
    OutPutArray(sendcounts, p);
    printf("disps, %d:", myid);
    OutPutArray(displs, p);
	
    MPI_Alltoall(sendcounts, 1, MPI_INT, 
                 recvcounts, 1, MPI_INT, MPI_COMM_WORLD);
    redispls[0]=0;
    size=recvcounts[0];
    for(i=1; i<p; i++)
    {
        redispls[i]=redispls[i-1]+recvcounts[i-1];
        size+=recvcounts[i];
    }
	
    MPI_Alltoallv(A, sendcounts, displs, MPI_INT, 
                  tmp, recvcounts, redispls, MPI_INT, MPI_COMM_WORLD);
	
    for(i=0; i<size; i++)
        A[i]=tmp[i];
	
    free(tmp);
    free(sendcounts);
    free(recvcounts);
    free(displs);
    free(redispls);
	
    return size;
}

int main(int argc, char *argv[])
{
    int myid, numprocs;
    MPI_Status status;
    int i, n, size, tmp, *A;
		
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	
    if(0==myid)
    {
        n = readFromFile(&A);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    size=n/numprocs;
    if(0!=myid)
    {
        A=(int*)malloc(sizeof(int)*size*2);
        MPI_Recv(A, size, MPI_INT, 0, TAG_A, MPI_COMM_WORLD, &status);
    }
    if(0==myid)
    {
        tmp=size;
        size=n-(numprocs-1)*size;
        for(i=0; i<numprocs-1; i++)
            MPI_Send(A+size+tmp*i, tmp, MPI_INT, i+1, TAG_A, MPI_COMM_WORLD);
    }
    size=QuickSort(A, size, n, myid, numprocs);
	
    if(myid!=0)
        MPI_Send(A, size, MPI_INT, 0, TAG_A, MPI_COMM_WORLD);
    if(0==myid)
    {
        tmp=size;
        for(i=1; i<numprocs; i++)
        {
            MPI_Recv(A+tmp, n, MPI_INT, i, TAG_A, MPI_COMM_WORLD, &status);
            tmp+=status.count/sizeof(int);
        }
        printf("After sorted:\n");
        OutPutArray(A, n);
    }
    free(A);
    MPI_Finalize();	
}

void RandArray(int *A, int n,  int seed)  
{
    int i;
    srand(seed);
    for(i=0; i<n; i++)
        A[i]=rand();
}

void OutPutArray(int *A, int n)
{
    int i;
    for(i=0; i<n; i++)
        printf("%d  ", A[i]);
    printf("\n");
}

int readFromFile(int **A )
{
    int i, n;
    FILE* fin = fopen("dataIn.txt", "r");
	
    if (fin == NULL)
    {
        printf( "Cannot find input data file\nPlease create a file \"dataIn.txt\"\n");
        return -1;
    }
	
    fscanf(fin, "%d\n",  &n);
	
    if ((n < 1)||(n > MAX_DATA_SIZE))
    {
        printf("Data size out of range(1~%d)!\n", MAX_DATA_SIZE);
        return -1;
    }
    *A=(int*)malloc(sizeof(int)*n);
    for(i = 0; i < n; i ++)
    {
        fscanf(fin, "%d", (*A)+i);
    }
	
    fclose(fin);
	
    printf("Read from data file \"dataIn.txt\"\n");
    OutPutArray(*A, n);
	
    return n;
}


