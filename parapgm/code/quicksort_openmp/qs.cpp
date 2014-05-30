// omp_section.cpp : 定义控制台应用程序的入口点。
//
#include <stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<time.h>

int Partition(int *data,int start,int end)
{
    int pivo;
    int i, j;
    int tmp;
	
    pivo=data[end];			
    i=start-1;				/*i(活动指针)*/
	
    for(j=start;j<end;j++)
        if(data[j]<=pivo)
        {
            i++;			/*i表示比pivo小的元素的个数*/
            tmp=data[i];
            data[i]=data[j];
            data[j]=tmp;
        }
		
    tmp=data[i+1];
    data[i+1]=data[end];
    data[end]=tmp;			/*以pivo为分界，data[i+1]=pivo*/
		
    return i;
}
int* QuickSort_parallel(int* Data, int Begin, int End)
{
    int r;
    if(Begin<End)
    {
        r=Partition(Data, Begin, End);
#pragma omp parallel 
        {
#pragma omp sections nowait
        {
#pragma omp section
            QuickSort_parallel(Data,Begin,r);
#pragma omp section
            QuickSort_parallel(Data,r+1,End);
        }
    }
}
    return Data;
}

int main(int argc, char *argv[])
{
    int N;
    printf("输入个数：");
    scanf("%d",&N);
    //for(int i=0;i<N;i++)
    //	cout<<rand()<<endl;
	
    int *Data;
    Data=new int[N];
    for(int i=0;i<N;i++)
        Data[i]=rand();
    //for(int i=0;i<N;i++)
    //cout<<Data[i]<<endl;
    clock_t timeBegin =clock();
    Data=QuickSort_parallel(Data,0,N-1);
    clock_t timeFinish =clock();
    
    // for (int i=0;i<N;i++)
    // {
    //     printf("%d\n",Data[i]);
    // }
	
    printf("并行快速排序所花时间：%ld\n",timeFinish-timeBegin);
}
