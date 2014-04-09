#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define MAX_N 50
#define PI 3.14159
#define EPS 10E-8
#define V_TAG 110
#define P_TAG 119
#define Q_TAG 120
#define R_TAG 911
#define S_TAG 111
#define S_TAG2 122

typedef enum {FALSE,TRUE} BOOL;

typedef struct 
{
    double real;
    double imag;
}compl;

compl p[MAX_N],s[2*MAX_N],r[2*MAX_N];
compl w[2*MAX_N];
int variableNum;
double transTime=0,totalTime=0,beginTime;
MPI_Status status;

void comp_add(compl* result,const compl* c1,const compl* c2)
{
    result->real=c1->real+c2->real;
    result->imag=c1->imag+c2->imag;
}

void comp_multiply(compl* result,const compl* c1,const compl* c2)
{
    result->real=c1->real*c2->real-c1->imag*c2->imag;
    result->imag=c1->real*c2->imag+c2->real*c1->imag;
}

void shuffle(compl* f,int beginPos,int endPos)
{
	int i;
	compl temp[2*MAX_N];
	for(i=beginPos;i<=endPos;i++)
	{
		temp[i]=f[i];
	}	
	int j=beginPos;
	for(i=beginPos;i<=endPos;i+=2)
	{
		f[j]=temp[i];
        j++;
	}
    for(i=beginPos+1;i<=endPos;i+=2)
	{
		f[j]=temp[i];
	    j++;
	}
}

void evaluate(compl*f,int beginPos,int endPos,const compl* x,compl* y,
int leftPos,int rightPos,int totalLength)
{
	int i;
	if((beginPos>endPos)||(leftPos>rightPos))
	{
		printf("Error in use Polynomial!\n");
        exit(-1);
	}
	else if(beginPos==endPos)
	{
		for(i=leftPos;i<=rightPos;i++)
		{
		  y[i]=f[beginPos];
		}   
	}
	else if(beginPos+1==endPos)
	{
		for(i=leftPos;i<=rightPos;i++)
		{
		  compl temp;
		  comp_multiply(&temp,&f[endPos],&x[i]);
		  comp_add(&y[i],&f[beginPos],&temp);
		}   
	}
	else
	{
	compl tempX[2*MAX_N],tempY1[2*MAX_N],tempY2[2*MAX_N];
	int midPos=(beginPos+endPos)/2;
	shuffle(f,beginPos,endPos);
	for(i=leftPos;i<=rightPos;i++)
	{
	  comp_multiply(&tempX[i],&x[i],&x[i]);
	}
	evaluate(f,beginPos,midPos,tempX,tempY1,leftPos,rightPos,totalLength);
	evaluate(f,midPos+1,endPos,tempX,tempY2,leftPos,rightPos,totalLength);
   for(i=leftPos;i<=rightPos;i++)
   {
	 compl temp;
     comp_multiply(&temp,&x[i],&tempY2[i]);
     comp_add(&y[i],&tempY1[i],&temp);
   }  
	}
}
void print(const compl* f,int fLength)
{
	BOOL isPrint=FALSE;
	int i;
	if(fabs(f[0].real)>EPS)
	{
	  printf("%f",f[0].real);
	  isPrint=TRUE;
	}
	for(i=1;i<fLength;i++)
	{
		if(f[i].real>EPS)
		{
			if(isPrint)
        printf("+");
			else
        isPrint=TRUE;
	    printf("%ft^%d",f[i].real,i);
		}
	else if(f[i].real<-EPS)
	{
		if(isPrint)
     printf("-");
		else
     isPrint=TRUE;
	 printf("%ft^%d",-f[i].real,i);
	}
	}
	if(isPrint==FALSE)
      printf("0");
	printf("\n");
}


void printres(const compl* f,int fLength)
{
	int i;
	for(i=0;i<fLength;i+=2)
	{		if(f[i].imag<0)
               printf("%f-%fi\n",f[i].real,-f[i].imag);
              else
		printf("%f+%fi\n",f[i].real,f[i].imag);
	}
	printf("\n");
}

void addTransTime(double toAdd)
{
	transTime+=toAdd;
}

BOOL readFromFile()
{
	int i;
        printf("Read from data file \"dataIn.txt\"\n");
	FILE* fil=fopen("dataIn.txt","r");
	if(fil==NULL)
	{
		printf("Cannot open input data file"
		"Please create a file\"dataIn.txt\"\n");
		return(FALSE);
	}
	fscanf(fil,"%d \n",&variableNum);
	if((variableNum<1)||(variableNum>MAX_N))
	{
		printf("variableNum out of range! \n");
		return(FALSE);
	}
	printf("variableNum=%d\n",variableNum);	
	for(i=0;i<variableNum;i++)
	{
		fscanf(fil,"%lf",&p[i].real);
	        printf("p[%d].real=%lf   ",i,p[i].real);
		p[i].imag=0.0;
	}
	printf("\n");
	fclose(fil);	
	printf("p(t)=");
	print(p,variableNum);
	return(TRUE);
}



int main(int argc,char * argv[])
{
	int rank,size,i;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	if(rank==0)
	{
		if(!readFromFile())
			exit(-1);
		if(size>2*variableNum)
		{
			printf("Too many Processors,reduce your -np value \n");
			MPI_Abort(MPI_COMM_WORLD,1);
		}
		beginTime=MPI_Wtime();
		for(int i=1;i<size;i++)
		{
		MPI_Send(&variableNum,1,MPI_INT,i,V_TAG,MPI_COMM_WORLD);
		MPI_Send(p,variableNum*2,MPI_DOUBLE,i,P_TAG,MPI_COMM_WORLD);
		}
		
		addTransTime(MPI_Wtime()-beginTime);
	}
	else
	{
	    MPI_Recv(&variableNum,1,MPI_INT,0,V_TAG,MPI_COMM_WORLD,&status);
	    MPI_Recv(p,variableNum*2,MPI_DOUBLE,0,P_TAG,MPI_COMM_WORLD,&status);
	}
	int wLength=2*variableNum;
	for(i=0;i<wLength;i++)
	{
		w[i].real=cos(i*2*PI/wLength);
		w[i].imag=sin(i*2*PI/wLength);
	}
	int everageLength=wLength/size;
	int moreLength=wLength%size;
	int startPos=moreLength+rank*everageLength;
	int stopPos=startPos+everageLength-1;
	if(rank==0)
	{
		startPos=0;
		stopPos=moreLength+everageLength-1;
	}
	evaluate(p,0,variableNum-1,w,s,startPos,stopPos,wLength);
       
	if(rank>0)
	{
		MPI_Send(s+startPos,everageLength*2,MPI_DOUBLE,0,S_TAG,MPI_COMM_WORLD);
		MPI_Recv(s,wLength*2,MPI_DOUBLE,0,S_TAG2,MPI_COMM_WORLD,&status);
	}
	else
	{
		double tempTime=MPI_Wtime();
		for(i=1;i<size;i++)
		{
			MPI_Recv(s+moreLength+i*everageLength,everageLength*2,MPI_DOUBLE,i,S_TAG,MPI_COMM_WORLD,&status);
		}
		for(i=1;i<size;i++)
		{
			MPI_Send(s,wLength*2,MPI_DOUBLE,i,S_TAG2,MPI_COMM_WORLD);
		}
	      printf("The final results :\n");
	      printres(s,wLength);
		addTransTime(MPI_Wtime()-tempTime);
	}	
	if(rank==0)
	{
	        totalTime=MPI_Wtime();
	        totalTime-=beginTime;
		printf("\nUse prossor size=%d\n",size);
		printf("Total running time=%f(s)\n",totalTime);
		printf("Distribute data time = %f(s)\n",transTime);
		printf("Parallel compute time = %f(s)\n ",totalTime-transTime);
	}
	MPI_Finalize();
	return 0;
}

