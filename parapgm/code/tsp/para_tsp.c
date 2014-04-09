#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

//#define GENERATION_NUM	50
//#define CITIES	6
//#define POPULATION_SIZE	20

#define REQUEST_PATH_TAG (POPULATION_SIZE+1)
#define DONE_TAG (POPULATION_SIZE+2)


//Travelling sales man problem , travelling between 6 cities.

//initializing the path lengths between cities and the paths to be included
//in population

int myrank,numprocs;

int GENERATION_NUM,CITIES,POPULATION_SIZE;
int generation_end ;

int pre_init(int ***pathlen,int ***path,int **fx,int ***child,int **need_eva_path){

	//For All Nodes

	//pathlen[CITIES][CITIES]
	*pathlen = (int **)malloc(sizeof(int *)*CITIES);
	if(*pathlen == NULL){
		printf("Malloc error!");
		return -1;
	}

	for(int i=0;i<CITIES;i++){
		(*pathlen)[i] = (int *)malloc(sizeof(int)*CITIES);
		if((*pathlen)[i] == NULL){
			printf("Malloc error!");
			return -1;
		}
	}
	
	//need_eva_path[CITIES]
	*need_eva_path = (int *)malloc(sizeof(int *)*CITIES);
	if(*need_eva_path == NULL){
		printf("Malloc error!");
		return -1;
	}
	

	if(myrank==0){

		//path[POPULATION_SIZE][CITIES]
		*path = (int **)malloc(sizeof(int *)*POPULATION_SIZE);
		if(*path == NULL){
			printf("Malloc error!");
			return -1;
		}

		for(int i=0;i<POPULATION_SIZE;i++){
			(*path)[i] = (int *)malloc(sizeof(int)*CITIES);
			if((*path)[i] == NULL){
				printf("Malloc error!");
				return -1;
			}
		}	
	
		//fx[POPULATION_SIZE]
		*fx = (int *)malloc(sizeof(int)*POPULATION_SIZE);
		if(*fx == NULL){
				printf("Malloc error!");
				return -1;
			
		}

		//child[2][CITIES]
		*child = (int **)malloc(sizeof(int *)*2);
		if(*child == NULL){
			printf("Malloc error!");
			return -1;
		}

		for(int i=0;i<2;i++){
			(*child)[i] = (int *)malloc(sizeof(int)*CITIES);
			if((*child)[i] == NULL){
				printf("Malloc error!");
				return -1;
			}
		}	
		
		
		system("clear");
		srandom(0/*(int)time(0)*/);

		int cities=CITIES;
		int population_size=POPULATION_SIZE;


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
	}

	return 0;
}


void disposal(int **pathlen,int **path,int *fx,int **child,int *need_eva_path){

	//For All Nodes
	for(int i=0;i<CITIES;i++)
		free(pathlen[i]);
	free(pathlen);
	
	free(need_eva_path);

	if(myrank == 0){
		free(child[0]);
		free(child[1]);
		free(child);

		free(fx);

		for(int i=0;i<CITIES;i++)
			free(path[i]);
		free(path);
	}

}


void initialize(int **pathlen,int **path)
{
	int i,j,k;
	if(myrank == 0){

		//obtaining pathlengths
		for(i=0;i<CITIES;i++)
		{
			for(j=0;j<CITIES;j++)
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
	}

	for(i=0;i<CITIES;i++)
		MPI_Bcast(pathlen[i],CITIES,MPI_INT,0,MPI_COMM_WORLD);

	if(myrank == 0){
		// display the path lengths

		printf("\n\tThe PATH LENGTHS ARE: \n" );

		for(i=0;i<CITIES;i++)
		{
			for(j=0;j<CITIES;j++)
			{
				printf(" %5d ",pathlen[i][j]);
			}
			printf("\n\n");
		}
		

		// generating the population

		for(i=0;i<POPULATION_SIZE;i++)
		{
			for(j=0;j<CITIES;j++)
			{
				path[i][j]=random()%CITIES;

				for(k=j-1;k>=0;k--)
				{
					if(path[i][j]==path[i][k])  //checking to avoid repeatition
					{
						path[i][j] = random()%CITIES;
						k=j;
					}
				}
			}
		}

	}

}

// evaluating the fitness function or total distance

void distribute(int **path,int *fx)
{
	MPI_Status status;
	
	int sum = 0, index = 0;

	while(1){

		MPI_Recv(&sum,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);

		if(status.MPI_TAG == REQUEST_PATH_TAG){
			MPI_Send(path[index],CITIES,MPI_INT,status.MPI_SOURCE,index,MPI_COMM_WORLD);
			index ++;

			if(index == POPULATION_SIZE)
				break;
		}else{
			fx[status.MPI_TAG] = sum;
		}
	}


	//display the paths generated
	/*printf("\n");
	printf("\n\tPATH \t\tf(x) \n\n");
	for(int i=0;i<POPULATION_SIZE;i++)
	{
		printf("\t");
		for(int j=0;j<CITIES;j++)
		{
			printf(" %d",path[i][j]);
		}
		printf("\t%d",fx[i]);
		printf("\n");
	}
	printf("\n\n\n\tPRESS ANY KEY TO CONTINUE.");
	*/
}

void evaluate(int **pathlen,int *need_eva_path)
{

	MPI_Status status;

	//printf("Evaluate start in process %d",myrank);

	MPI_Send(NULL,0,MPI_INT,0,REQUEST_PATH_TAG,MPI_COMM_WORLD);

	while(1){

		MPI_Recv(need_eva_path,CITIES,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);

		if(status.MPI_TAG == DONE_TAG){

			//printf("Processer %d received done tag!\n",myrank);
			break;

		}else{

			int sum = 0;
			int a,b;

			for(int i = 0;i < CITIES;i ++){
				a = need_eva_path[i]; 
				b = need_eva_path[(i+1)%CITIES];
				sum += pathlen[a][b];
			}

			MPI_Send(&sum,1,MPI_INT,0,status.MPI_TAG,MPI_COMM_WORLD);
		}

		MPI_Send(NULL,0,MPI_INT,0,REQUEST_PATH_TAG,MPI_COMM_WORLD);
	}

}

void terminate(){

	MPI_Status status;
	
	int temp,count=0;

	while(1){
		MPI_Recv(&temp,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);

		if(status.MPI_TAG == REQUEST_PATH_TAG){
			MPI_Send(NULL,0,MPI_INT,status.MPI_SOURCE,DONE_TAG,MPI_COMM_WORLD);
			count ++;	
		}
		if(count == numprocs - 1)
			break;
	}
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
	for(i=0;i<POPULATION_SIZE;i++)
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
	int crosspt1,crosspt2,j,i,temp,temp1[2][CITIES],temp2;
	//TAKING 2 CROSS POINTS
	do
	{
		crosspt1=random()%(CITIES-1);
		crosspt2=random()%(CITIES-1);
	}while(crosspt1==crosspt2);
	if(crosspt1 > crosspt2){
		temp = crosspt1;
		crosspt1=crosspt2;
		crosspt2=temp;
	}

	/*system("clear");
	printf("\n\n\t The CROSSOVER POINTS ARE : %d , %d ",crosspt1,crosspt2);
	printf("\n\n\tTHE PATHS FOR CROSSOVER ARE");
	printf("\n\n\t\t");*/

	for(j=0;j<CITIES;j++)
	{
		child[0][j]=path[pos[0]][j];
		//printf(" %d",child[0][j]);
	}
	//printf("\n\t\t");
	for(j=0;j<CITIES;j++)
	{
		child[1][j]=path[pos[1]][j];
		//printf(" %d",child[1][j]);
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

						for(k=0;k<CITIES;k++)
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
						for(k=0;k<CITIES;k++)
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
		for(i=crosspt2+1;i<CITIES;i++)   //now chehcking the path after the second cross point
		{
			for(j=0;j<cnt;j++)   //comparing the path within crossover point
			{
				if(child[m][i]==temp1[m][j])  //if found then
				{
					if(m==0)   //for child 1
					{
						temp2=temp1[1][j];   //take the path from child2 crossove
						for(k=0;k<CITIES;k++)
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
	//display AfTER  CROSSOVER
	/*printf("\n\tAFTER CROSSOVER\n\t\t");

	for(j=0;j<CITIES;j++)
	{
		printf(" %d",child[0][j]);
	}
	printf("\n\t\t");
	for(j=0;j<CITIES;j++)
	{
		printf(" %d",child[1][j]);
	}

	printf("\n\n\n\tPRESS ANY KEY TO CONTINUE.");
	*/
}

//insering the paths in population removing those having maximum populaiton

void insert(int **child,int posmax[2],int **path)
{
	for(int j=0;j<CITIES;j++)
	{
		path[posmax[0]][j]=child[0][j];
		path[posmax[1]][j]=child[1][j];
	}


}

// performing mutation

void mutation(int **path)
{
	int sel=random()%POPULATION_SIZE;
	int pos1=random()%CITIES;
	int pos2=random()%CITIES;
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

	CITIES = atoi(argv[1]);
	POPULATION_SIZE = atoi(argv[2]);
	if(argc == 4)
		GENERATION_NUM = atoi(argv[3]);

	char processor_name[MPI_MAX_PROCESSOR_NAME];
	double	start_time=0.0,end_time;
	int namelen;

	int res;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Get_processor_name(processor_name,&namelen);

	printf("Processor %d of %d is on %s\n",myrank,numprocs,processor_name);

	if(numprocs < 2){
		printf("At least 2 processes are required!");	
		exit(-1);
	}

	//int pathlen[CITIES][CITIES],path[POPULATION_SIZE][CITIES],fx[POPULATION_SIZE],pos[2],posmax[2],child[2][CITIES],need_eva_path[CITIES];
	int **pathlen,**path,*fx,**child,*need_eva_path;

	int pos[2],posmax[2];

	//·Öä´¢¿ռäres=pre_init(&pathlen,&path,&fx,&child,&need_eva_path);
	if(res == -1){
		printf("\nError in pre_init function!");
		return -1;
	}
	initialize(pathlen,path);

	if(myrank == 0){

		int iteration_count=0;
		GENERATION_NUM = 0;

		start_time = MPI_Wtime();

		for(int i=1;/*i < GENERATION_NUM*/;i++)
		{
			distribute(path,fx);
			selection(fx,pos,posmax);

			if(generation_end == 1){
				iteration_count = i;
				break;
			}

			crossover(pos,path,child);
			insert(child,posmax,path);
			mutation(path);
		}

		end_time = MPI_Wtime();
		
		printf("The best travel path:\n");
       		for(int j=0;j<CITIES;j++){
                	printf("%d\t",path[pos[0]][j]);
        	}	
        	printf("\nThis path has the shortest length %d\n",fx[pos[0]]);
		printf("\nGeneration count:%d",iteration_count);
		printf("\nTime use:%f seconds\n\n",end_time-start_time);

		terminate();
	
	}else{
		evaluate(pathlen,need_eva_path);
	}

	disposal(pathlen,path,fx,child,need_eva_path);

	MPI_Finalize();
	return 0;

}


