/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"
#include <fcntl.h>/* required for O_CREAT */
#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;
sem_t *sem;
pthread_mutex_t mutex;
// the work queue
task worktodo[QUEUE_SIZE];
int front,rear;
pthread_t bee[NUMBER_OF_THREADS];// the worker bee, i.e. the threads to be created by pool_init

/*bool isEmpty(){
    return front == rear;
}*/

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
    if((rear+1) % QUEUE_SIZE != front){  // queue is not full
    	rear = (rear + 1) % QUEUE_SIZE;
	worktodo[rear].function = t.function;
    	worktodo[rear].data = t.data;
	return 0;
    }
    else return 1;
}

// remove a task from the queue
task dequeue() // Here there should be considerations about whether the queue is empty, but in this project it is does not matter if we ignore it
{
    front = (front + 1) % QUEUE_SIZE;
    return worktodo[front];
}

// the worker thread in the thread pool
void *worker(void *param)
{
    task tmpr;

    /* execute the task   */ 
 
    sem_wait(sem);  // acquire the semaphore, if number of tasks in the queue is 0, it waits
			// sem_wait(sem) substract 1 from the number of tasks in the queue

    /* mutex locks to avoid the race conditions */   
    pthread_mutex_lock(&mutex);// acquire the mutex lock
    tmpr = dequeue();// critical section
    pthread_mutex_unlock(&mutex);// release the mutex lock
    

    execute(tmpr.function, tmpr.data);

    pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    int flag;
    task tmp;
    tmp.function = somefunction;
    tmp.data = p;
    
    /* mutex locks to avoid the race conditions */
    pthread_mutex_lock(&mutex);// acquire the mutex lock    
    flag = enqueue(tmp);// critical section
    pthread_mutex_unlock(&mutex);// release the mutex lock

    if(flag == 0){    
	printf("enqueue successfully\n");
	sem_post(sem); // release the semaphore
		       // the times of sem_post executed means the number of tasks enqueued
	return 0;
    }
    else{
	printf("enqueue error\n");
	return 1;
    }

   

    return 0;
}

// initialize the thread pool
void pool_init(void)
{
    front = rear = 0; 
    sem = sem_open("SEM", O_CREAT, 0666, 0);// Create or open the semaphore and initialize it to 0

    pthread_mutex_init(&mutex,NULL);// create and initialize the mutex lock 

    for(int l = 0; l < NUMBER_OF_THREADS; l++)
    	pthread_create(&bee[l],NULL,worker,NULL); //choose a task from the queue's front
}

// shutdown the thread pool
void pool_shutdown(void)
{
    for(int k = 0;k < NUMBER_OF_THREADS;++k) //cancel the thread
	pthread_cancel(bee[k]);
    for(int k = 0;k < NUMBER_OF_THREADS;++k) //wait for each thread to terminate
    	pthread_join(bee[k],NULL);
}
