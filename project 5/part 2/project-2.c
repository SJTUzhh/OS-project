#include <stdlib.h> /* required for rand() */
#include <stdio.h> /* required for atoi() */
#include <pthread.h> /*required for creating and manipulating muti-threads */
#include <semaphore.h> /* required for creating semaphores and calling sem_wait() and sem_post() */
#include <unistd.h>/* required for sleep() */
#include <fcntl.h>/* required for O_CREAT */
#define BUFFER_SIZE 3
#define true 1
#define MAX 5 //thread's max sleeping time 
#define max_num_of_producer_threads 8
#define max_num_of_consumer_threads 8
sem_t *mutex;
sem_t *empty;
sem_t *full;
int numpro;
int numcon;
pthread_t pro[max_num_of_producer_threads];
pthread_t con[max_num_of_consumer_threads];
/* data structures of buffer */
typedef int buffer_item;
buffer_item buffer[BUFFER_SIZE];
int front,rear; // buffer is designed to be a circular queue

/* functions used on buffer */
/* insert item into buffer
	return 0 if successful, otherwise
	return -1 indicating an error condition */
int insert_item(buffer_item item) { 
	if((rear+1) % BUFFER_SIZE != front){  // queue is not full
    		rear = (rear + 1) % BUFFER_SIZE;
		buffer[rear] = item;
		return 0;
    	}
    	else return -1;	
}
/* remove an object from buffer placing it in item
	return 0 if successful, otherwise
	return -1 indicating an error condition */
int remove_item(buffer_item *item) {
	if(rear != front) {
		front = (front + 1) % BUFFER_SIZE;
		*item = buffer[front];
		return 0;
	}
	else return -1;
    		
}
/* initialize buffer */
void buffer_init(){
	front = rear = 0; 
	mutex = sem_open("MUTEX", O_CREAT, 0666, 1);
	empty = sem_open("EMPTY", O_CREAT, 0666, BUFFER_SIZE);
	full = sem_open("FULL", O_CREAT, 0666, 0);
}

/* producer */
void *producer(void *param) {
	buffer_item item;
	int tmp1;
	while (true) {
		sleep(rand()%MAX);// sleep for a random period of time
	
		item = rand();// produce an item in next_produced
 
		sem_wait(empty);
		sem_wait(mutex);
	
		tmp1 = insert_item(item);// add next_produced to the buffer
		
		sem_post(mutex);
		sem_post(full); 

		if(tmp1) printf("buffer is full\n");
		else printf("producer produced %d\n",item);
	}
	pthread_exit(0);//easy to forget
}
/*consumer*/
void *consumer(void *param) {
	buffer_item item;
	int tmp2;
	while (true) {		
		sleep(rand()%MAX);// sleep for a random period of time
		
		sem_wait(full);
		sem_wait(mutex);

		tmp2 = remove_item(&item);// remove an item from buffer to next_consumed
		
		sem_post(mutex);
		sem_post(empty);

		if (tmp2) printf("buffer is empty\n");
		else printf("consumer consumed %d\n",item);// consume the item in next_consumed/
	}
	pthread_exit(0);//easy to forget
}

int main(int argc, char *argv[]) {
	int sleeptime = atoi(argv[1]); // get the parameters
  	numpro = atoi(argv[2]);
	numcon = atoi(argv[3]);
	buffer_init();  //initialize buffer and the three semaphores
	/*pthread_t pro[numpro];
	pthread_t con[numcon];*/
	for(int i = 0; i < numpro; i++) // creat the producer threads
    		pthread_create(&pro[i],NULL,producer,NULL);
	for(int i = 0; i < numcon; i++) // creat the consumer threads
    		pthread_create(&con[i],NULL,consumer,NULL);

	srand((unsigned)time(NULL));
	sleep(sleeptime);

	for(int i = 0;i < numpro ;++i) //cancel the producer thread
		pthread_cancel(pro[i]);
	for(int i = 0;i < numcon ;++i) //cancel the consumer thread
		pthread_cancel(con[i]);
    	for(int i = 0;i < numpro ;++i) //wait for each produce thread to terminate
		pthread_join(pro[i],NULL);
	for(int i = 0;i < numcon ;++i) //wait for each consumer thread to terminate		
    		pthread_join(con[i],NULL);

	return 0;
}
/* 1. Get command line arguments argv[1],argv[2],argv[3] */
/* 2. Initialize buffer */
/* 3. Create producer thread(s) */
/* 4. Create consumer thread(s) */
/* 5. Sleep */
/* 6. Exit */

/*producer*/
/*while (true) {
. . .
/* produce an item in next_produced */
/*. . .
wait(empty);
wait(mutex);
. . .
/* add next_produced to the buffer */
/*. . .
signal(mutex);
signal(full);
}



/*consumer*/
/*while (true) {
wait(full);
wait(mutex);
. . .
/* remove an item from buffer to next consumed */
/*. . .
signal(mutex);
signal(empty);
. . .
/* consume the item in next_consumed */
/*. . .
}*/











