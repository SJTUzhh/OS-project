/* use openmp to implement quicksort */
#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<time.h>
//#define NUM_THREADS 8
#define ARRAY_COUNT 20000
#define MAX 100 // upper bound of the element number
#define THRESHOLD 500 // when to call simpler sort functions
/* function that can generate a random array */
int Data[ARRAY_COUNT];
int Data1[ARRAY_COUNT];
void initElem(){
    	srand((unsigned)time(NULL));
    	int i;
    	for(i=0;i<ARRAY_COUNT;i++){
        	Data[i]=rand()%MAX+1;
    	}
}
/* function used for printting array */
void print(int *array){
        int i;
        for(i=0;i<ARRAY_COUNT;i++)
                printf("%d ",array[i]);
        printf("\n");
}
void bubble_sort(int a[],int l,int r) // a simpler sortting algorithm
{
    	int i,j;
    	char t;
    	for(i=l;i<r;i++){   	
        	for(j=l;j<r-i;j++){
            		if(a[j]>a[j+1]){
               			t=a[j+1];
               			a[j+1]=a[j];
               			a[j]=t;
            		}
        	}
    	}
    	return;
}
int Partition(int *data,int p,int r) // used for quicksort
{
	int x,i,j,tmp;
	x=data[r];			
	i=p-1;				
	for(j=p;j<r;j++)
		if(data[j]<=x)
		{
			i++;
			//exchange A[i] and A[j]
			tmp=data[i];   
			data[i]=data[j];
			data[j]=tmp;
		}
		//exchange A[i+1] and A[r]
		tmp=data[i+1];
		data[i+1]=data[r];
		data[r]=tmp;
 
		return i+1;
}
int* QuickSort_parallel(int* Data,int p,int r)
{
	int q;
	if(p<r){
		if((r-p) <= THRESHOLD){ // use simpler algorithm. Experiments shows that the value of THRESHOLD has a great impact on parallel performance
			bubble_sort(Data,p,r);	
		}
		else{
			q=Partition(Data,p,r);
        	#pragma omp parallel 
			{
          	#pragma omp sections nowait
				{
             	 	#pragma omp section
			     	QuickSort_parallel(Data,p,q-1);
              		#pragma omp section
			     	QuickSort_parallel(Data,q+1,r);
				}
			}
		}
	}
	return Data;
}
int* QuickSort(int* Data,int p,int r)
{
	int q;
	if(p<r)
	{
		q=Partition(Data,p,r);
		QuickSort(Data,p,q-1);
		QuickSort(Data,q+1,r);	
	}
	return Data;
}
void main()
{
	int *sortedData,*sortedData1;
	initElem();
	/*printf("array: ");
	print(Data);*/
	for(int j=0;j<ARRAY_COUNT;++j) // Data's backup
		Data1[j]=Data[j];
	/* parallel */
	clock_t timep1 =clock(); 
	sortedData = QuickSort_parallel(Data,0,ARRAY_COUNT-1);
	clock_t timeFinish1 =clock();
	/* serial */
	clock_t timep2 =clock();
	sortedData1 = QuickSort(Data1,0,ARRAY_COUNT-1);
	clock_t timeFinish2 =clock();	
	

	/*printf("sorted array parallel: ");
	print(sortedData);
	printf("sorted array serial: ");
	print(sortedData1);*/

	printf("time used when parallel：%ld us\n",timeFinish1-timep1);
	printf("time used when serial：%ld us\n",timeFinish2-timep2);
}

