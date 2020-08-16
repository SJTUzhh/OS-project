/* use openmp to implement quicksort */
#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<time.h>
//#define NUM_THREADS 8
#define ARRAY_COUNT 10000
#define MAX 100 // upper bound of the element number
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
void merge(int array[], int left, int m, int right)
{
    	int aux[ARRAY_COUNT] = {0};
    	int i,j,k;  
    	for (i = left,j = m+1, k = 0; k <= right-left; k++){
        	if (i == m+1){
       			aux[k] = array[j++];
            		continue;
        	}
        	if (j == right+1){
            		aux[k] = array[i++];
            		continue;
        	}
        	if (array[i] < array[j]){        
            		aux[k] = array[i++];
        	}
        	else  aux[k] = array[j++];                  	
    	}    
    	for (i = left, j = 0; i <= right; i++, j++)   
        	array[i] = aux[j];    	
}
int *mergeSort(int *a, int start, int end) 
{ 
    	if(start < end) 
    	{ 
     		int middle = (start + end)/2; 
     		/* sort left half */ 
     		mergeSort(a, start, middle); 
     		/* sort right half */ 
    	 	mergeSort(a, middle + 1, end); 
     		/* merge the two halves */ 
    		merge(a, start, middle, end); 
    	}
	return a;
} 

int *mergesort_parallel(int *a, int start, int end) //parallelized version 
{          
    	//int threads = 8; 
    	if(start < end){
    	 	/*problem is here */
		if((end - start)<=300)  // use simpler algorithm. Experiments show that when to use has a great impact on performance
			bubble_sort(a,start,end); 
 
     		else {
			int middle = (start + end)/2; 
     			/*omp_set_nested(1);   
    	 		omp_set_num_threads(threads);*/ 
     			#pragma omp parallel sections 
     			{ 
     	 			#pragma omp section 
      	 			mergesort_parallel(a, start, middle); 
      				#pragma omp section 
       				mergesort_parallel(a, middle + 1, end); 
     			}
     			merge(a, start, middle,end);
		} 
    	} 
	return a;	
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
	sortedData = mergesort_parallel(Data,0,ARRAY_COUNT-1);
	clock_t timeFinish1 =clock();
	/* serial */
	clock_t timep2 =clock();
	sortedData1 = mergeSort(Data1,0,ARRAY_COUNT-1);
	clock_t timeFinish2 =clock();	
	

	/*printf("sorted array: ");
	print(sortedData);
	printf("sorted array: ");
	print(sortedData1);*/

	printf("time used when parallel：%ld us\n",timeFinish1-timep1);
	printf("time used when serial：%ld us\n",timeFinish2-timep2);
}

