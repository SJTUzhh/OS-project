#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define ARRAY_COUNT 10
#define MAX 100
int need_sort_array[ARRAY_COUNT];
int sorted[ARRAY_COUNT];
/* function that can generate a random array */
void initElem(){
    	srand((unsigned)time(NULL));
    	int i;
    	for(i=0;i<ARRAY_COUNT;i++){
        	need_sort_array[i]=rand()%MAX+1;
    	}
}
/* the insert-sorting function */
void *worker(void *param){
    	int i,j;
    	int tmp;
	int *part;
    	int left,right; // the two sides of the index
        /* determine which part to sort */
    	if(strcmp(param,"l") == 0) { left = 0; right = ARRAY_COUNT/2 - 1; }
    	else if(strcmp(param,"r") == 0) { left = ARRAY_COUNT/2; right = ARRAY_COUNT - 1; }
	/* the insert-sorting */
    	for(int i = left+1;i < right + 1;++i){
		tmp = need_sort_array[i];
		for(j = i-1;tmp < need_sort_array[j] && j >= left;--j)
	    		need_sort_array[j+1] = need_sort_array[j];
		need_sort_array[j+1] = tmp;
    	}
	pthread_exit(0);
}
/* the merge-sorting function */
void Merge_Array(){
        int i=0,j=ARRAY_COUNT/2,k=0;
        while(i<ARRAY_COUNT/2&&j<ARRAY_COUNT){
            if(need_sort_array[i]<need_sort_array[j])
                    sorted[k++]=need_sort_array[i++];
            else
                    sorted[k++]=need_sort_array[j++];
        }
        while(i<ARRAY_COUNT/2)
                sorted[k++]=need_sort_array[i++];
        while(j<ARRAY_COUNT)
                sorted[k++]=need_sort_array[j++];
}
/* function used for printting array */
void print(int *array){
        int i;
        for(i=0;i<ARRAY_COUNT;i++)
                printf("%d ",array[i]);
        printf("\n");
}

int main(int argc, char *argv[]){
	initElem();
	pthread_t workers[2]; /* the thread identifier */
	pthread_attr_t attr; /* set of attributes for the thread */
	pthread_attr_init(&attr);/* get the default attributes */

	printf("array: ");
	print(need_sort_array);

	/* create the thread */
	pthread_create(&workers[0],&attr,worker,"l");
	pthread_create(&workers[1],&attr,worker,"r");

	/* now wait for the threads to exit */
	for(int k = 0; k < 2 ; k++)
		pthread_join(workers[k],NULL);

	printf("partly sorted: ");
    	print(need_sort_array);
	
	Merge_Array();

    	printf("merged: ");
    	print(sorted);

	return 0;
}
