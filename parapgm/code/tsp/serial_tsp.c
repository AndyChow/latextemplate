#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

//#define GENERATION_NUM	50
//#define CITIES	6
//#define POPULATION_SIZE	20

int generation_num,cities,population_size;
int generation_end ;

//Travelling sales man problem , travelling between  cities.

//initializing the path lengths between cities and the paths to be included
//in population

void initialize(int **pathlen,int **path)
{
	int i,j,k;

	//obtaining pathlengths
	for(i=0;i<cities;i++)
	{
		for(j=0;j<cities;j++)
		{
			if(j<i)           //path length from a to b will be same as b to a
			{
				pathlen[i][j]=pathlen[j][i];
			}

			if(j==i)         // path length from a to a will be 0
			{
				pathlen[i][j]=0;

			}

			if(j>i)         // rest initialized
			{
				do{
				pathlen[i][j]= random()%50;
				}while(!pathlen[i][j]);
			}
		}

	}

	// display the path lengths

	/*printf("\n\tThe PATH LENGTHS ARE: \n" );

	for(i=0;i<cities;i++)
	{
		for(j=0;j<cities;j++)
		{
			printf(" %5d ",pathlen[i][j]);
		}
		printf("\n\n");
	}*/


	// generating the population

	for(i=0;i<population_size;i++)
	{
		for(j=0;j<cities;j++)
		{
			path[i][j]=random()%cities;

			for(k=j-1;k>=0;k--)
			{
				if(path[i][j]==path[i][k])  //checking to avoid repeatition
				{
					path[i][j] = random()%cities;
					k=j;
				}
			}
		}
	}

//	printf("\nInitial OK!");

}

// evaluating the fitness function or total distance

void evaluate(int **pathlen,int **path,int *fx)
{
	int sum =0,i,j,a,b;

	//obtaing the sum of the path taken
	for(i=0;i<population_size;i++)
	{
		sum=0;
		for(j=0;j<cities;j++)
		{
			a=path[i][j];
			b=path[i][(j+1)%cities];
			sum=sum+pathlen[a][b];
		}
		fx[i]=sum;

	}

	//display the paths generated
	/*printf("\n");
	printf("\n\tPATH \t\tf(x) \n\n");
	for(i=0;i<population_size;i++)
	{
		printf("\t");
		for(j=0;j<cities;j++)
		{
			printf(" %d",path[i][j]);
		}
		printf("\t%d",fx[i]);
		printf("\n");
	}
	printf("\n\n\n\tPRESS ANY KEY TO CONTINUE.");
	*/
}

//selecting the two points for cross over and then performing partial Crossover

void selection(int *fx,int pos[2],int posmax[2])
{
	int min1=fx[0],min2=fx[0],i,max1=fx[0],max2=fx[0];
	pos[0]=0;
	pos[1]=0;
	posmax[0]=0;
	posmax[1]=0;
	//calculating the minimum ,second minium,max,second max postion
	for(i=0;i<population_size;i++)
	{
		if(fx[i]<=min2)
		{
			if(fx[i]<=min1){
				min2=min1;
				pos[1]=pos[0];

				min1=fx[i];
				pos[0]=i;
			}else{
				min2=fx[i];
				pos[1]=i;
			}
		}

		if(fx[i]>=max2)
		{
			if(fx[i]>=max1){
				max2=max1;
				posmax[1]=posmax[0];		

				max1=fx[i];
				posmax[0]=i;
			}else{
				max2=fx[i];
				posmax[1]=i;
			}
		}
	}
	
	if(fx[pos[0]] == fx[posmax[0]])
		generation_end = 1;

	/*printf("\n\tFIRST MINIMUM=%4d \tPOSITION=%4d\n\tSECOND MINIMUN=%4d \tPOSITION=%4d\n\tFIRST MAXIMUM=%4d \tPOSITION=%4d\n\tSECOND MAXIMUM=%4d \tPOSITION=%4d\n",min1,pos[0],min2,pos[1],max1,posmax[0],max2,posmax[1]);

	printf("\n\n\n\tPRESS ANY KEY TO CONTINUE.");*/
}

//PERFORMING PARTIAL CROSSOVER

void crossover(int pos[2],int **path,int **child)
{
	int crosspt1,crosspt2,j,i,temp,/*temp1[2][CITIES],*/temp2;
	int **temp1;

	temp1 = (int **)malloc(sizeof(int *)*2);
	temp1[0] = (int *)malloc(sizeof(int)*cities);
	temp1[1] = (int *)malloc(sizeof(int)*cities);

	//TAKING 2 CROSS POINTS
	do
	{
		crosspt1=random()%(cities-1);
		crosspt2=random()%(cities-1);
	}while(crosspt1==crosspt2);
	if(crosspt1 > crosspt2){
		temp = crosspt1;
		crosspt1=crosspt2;
		crosspt2=temp;
	}

	//system("clear");
	//printf("\n\n\t The CROSSOVER POINTS ARE : %d , %d ",crosspt1,crosspt2);
	//printf("\n\n\tTHE PATHS FOR CROSSOVER ARE");
	//printf("\n\n\t\t");
	
	for(j=0;j<cities;j++)
	{
		child[0][j]=path[pos[0]][j];
	//	printf(" %d",child[0][j]);
	}
	//printf("\n\t\t");
	for(j=0;j<cities;j++)
	{
		child[1][j]=path[pos[1]][j];
	//	printf(" %d",child[1][j]);
	}

	int cnt=0;
	//swapping the paths between two crosspoints

	for(j=crosspt1+1;j<=crosspt2;j++)
	{
		temp1[1][cnt]=child[0][j];
		temp1[0][cnt]=child[1][j];
		temp=child[0][j];
		child[0][j]=child[1][j];
		child[1][j]=temp;
		cnt++;

	}
	//performing partial crossover

	int k,m;
	for(m=0;m<2;m++)
	{
		for(i=0;i<crosspt1+1;i++)   //taking the path before crosspoint
		{
			for(j=0;j<cnt;j++)   //comparing the path within crossover point
			{
				if(child[m][i]==temp1[m][j]) //if found then
				{
					if(m==0)   //for child 1
					{
						temp2=temp1[1][j];   //take the path from child2 crossover

						for(k=0;k<cities;k++)
						{
							if(child[m][k]==temp2) //if still the path repeats then repeat the process again
							{ temp2=child[1][k];
							  k=0;
							}
						}

						child[m][i]=temp2;   //finally putting the value in child

					}
					else  //for child 2
					{
						temp2=temp1[0][j];
						for(k=0;k<cities;k++)
						{
							if(child[m][k]==temp2)
							{temp2=child[0][k];
							 k=0;

							}
						}
						child[m][i]=temp2;
					}


				}


			}
		}
	}

	for(m=0;m<2;m++)
	{
		for(i=crosspt2+1;i<cities;i++)   //now chehcking the path after the second cross point
		{
			for(j=0;j<cnt;j++)   //comparing the path within crossover point
			{
				if(child[m][i]==temp1[m][j])  //if found then
				{
					if(m==0)   //for child 1
					{
						temp2=temp1[1][j];   //take the path from child2 crossove
						for(k=0;k<cities;k++)
						{
							if(child[m][k]==temp2) //if still the path repeats then repeat the process again
							{temp2=child[1][k];
							 k=0;
							 }
						}
						child[m][i]=temp2;  //finally assigning the value
					}
					else   //for child 2
					{

						temp2=temp1[0][j];
						for(k=0;k<cnt;k++)
						{
							if(child[m][k]==temp2)
							{temp2=child[0][k];
							 k=0;
							 }
						}
						child[m][i]=temp2;
					}

				}


			}
		}
	}
	//display AFTER  CROSSOVER
	/*printf("\n\tAFTER CROSSOVER\n\t\t");

	for(j=0;j<cities;j++)
	{
		printf(" %d",child[0][j]);
	}
	printf("\n\t\t");
	for(j=0;j<cities;j++)
	{
		printf(" %d",child[1][j]);
	}

	printf("\n\n\n\tPRESS ANY KEY TO CONTINUE.");
	*/
	free(temp1[0]);
	free(temp1[1]);
	free(temp1);
}

//insering the paths in population removing those having maximum populaiton

void insert(int **child,int posmax[2],int **path)
{
	for(int j=0;j<cities;j++)
	{
		path[posmax[0]][j]=child[0][j];
		path[posmax[1]][j]=child[1][j];
	}


}

// performing mutation

void mutation(int **path)
{
	int sel=random()%population_size;
	int pos1=random()%cities;
	int pos2=random()%cities;
	int temp=path[sel][pos1];
	path[sel][pos1]=path[sel][pos2];
	path[sel][pos2]=temp;
}

int main(int argc,char *argv[])
{
	if(argc != 4 && argc != 3){
		printf("Usage:serial_tsp cities population_size [generation number]");
		return -1;
	}

	cities = atoi(argv[1]);
	population_size = atoi(argv[2]);
	if(argc == 4)
		generation_num = atoi(argv[3]);

	system("clear");
	srandom(0/*(int)time(0)*/);

	//int pathlen[CITIES][CITIES],path[POPULATION_SIZE][CITIES],fx[POPULATION_SIZE],pos[2],posmax[2],child[2][CITIES];
	int **pathlen,**path,*fx,**child;

	int pos[2],posmax[2];

	int iteration_count=0;
	generation_num = 0;

	//pathlen[CITIES][CITIES]
	pathlen = (int **)malloc(sizeof(int *)*cities);
	if(pathlen == NULL){
		printf("Malloc error!");
		return -1;
	}

	for(int i=0;i<cities;i++){
		pathlen[i] = (int *)malloc(sizeof(int)*cities);
		if(pathlen[i] == NULL){
			printf("Malloc error!");
			return -1;
		}
	}	

	//path[POPULATION_SIZE][CITIES]
	path = (int **)malloc(sizeof(int *)*population_size);
	if(path == NULL){
		printf("Malloc error!");
		return -1;
	}

	for(int i=0;i<population_size;i++){
		path[i] = (int *)malloc(sizeof(int)*cities);
		if(path[i] == NULL){
			printf("Malloc error!");
			return -1;
		}
	}	
	
	//fx[POPULATION_SIZE]
	fx = (int *)malloc(sizeof(int)*population_size);
	if(fx == NULL){
		if(fx == NULL){
			printf("Malloc error!");
			return -1;
			}
	}

	//child[2][CITIES]
	child = (int **)malloc(sizeof(int *)*2);
	if(child == NULL){
		printf("Malloc error!");
		return -1;
	}

	for(int i=0;i<2;i++){
		child[i] = (int *)malloc(sizeof(int)*cities);
		if(child[i] == NULL){
			printf("Malloc error!");
			return -1;
		}
	}	
		

	printf("\n\t\t TRAVELLING SALESMAN PROBLEM ");
	printf("\n\t\t_____________________________");
	printf("\n\n\n\tTHE TRAVELLING SALES MAN PROBLEM DEALS WITH THE FACT");
	printf("\n\n\tTHAT A SALESMAN TRAVELS BETWEEN CITIES TAKING THE PATH");
	printf("\n\n\tTHAT IS OF MINIMUN DISTANCE AND THEN BACK TO THE START CITY.");

	printf("\n\n\n\tTO OBTAIN THE MINIMUM DISTANCE WE USE GENETIC ALGORITHM");
	printf("\n\n\tWHERE WE TAKE AN INITIAL POPULATION OF %d PATHS between %d cities",population_size,cities);
	printf("\n\n\tAND THEN THROUGH FITNESS FUNCTION WE OBTAIN THE PATHS ");
	printf("\n\n\tWITH MINIMUN DISTANCE AND REPLACE THEM WITH THE CHILDS ");
	printf("\n\n\tAFTER PARTIAL CROSSOVER WITH MAXIMUM DISTANCE.");

	
	printf("\n\n\tPRESS ANY KEY TO CONTINUE.");
	//getch();
	getchar();
	system("clear");
	initialize(pathlen,path);
	//evaluate(pathlen,path,fx);
	//getchar();
	//selection(fx,pos,posmax);
	//crossover(pos,path,child);
	//insert(childld,posmax,path);
	//mutation(path);
	struct timeval start_time,end_time;
	double timeuse;

	gettimeofday(&start_time,NULL);
	for(int i=1;;i++)
	{
		evaluate(pathlen,path,fx);
		selection(fx,pos,posmax);

		if(generation_end == 1){
			iteration_count = i;
			break;
		}

		crossover(pos,path,child);
		insert(child,posmax,path);
		mutation(path);
	}
	gettimeofday(&end_time,NULL);
	
	printf("The best travel path:\n");
	for(int j=0;j<cities;j++){
		printf("%d\t",path[pos[0]][j]);
	}
	printf("\nThis path has the shortest length %d\n",fx[pos[0]]);
	printf("\nGeneration count:%d",iteration_count);
	timeuse = (end_time.tv_sec - start_time.tv_sec)+(end_time.tv_usec - start_time.tv_usec)/1000000;
		
	printf("\nTime use:%f seconds \n\n",timeuse);
	//crossover(pos,path,child);
	//insert(child,posmax,path);

	//evaluate(pathlen,path,fx);
	//printf("\n\n\n\tPRESS ANY KEY TO END.");
	free(child[0]);
	free(child[1]);
	free(child);

	free(fx);

	for(int i=0;i<cities;i++)
		free(path[i]);
	free(path);

	for(int i=0;i<cities;i++)
		free(pathlen[i]);
	free(pathlen);
	
	//getchar();

	return 0;
}


