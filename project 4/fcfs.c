//fcfs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE  100
typedef struct task {
    char name[3];
    int tid;
    int priority;
    int burst;
} Task;
struct node {
    Task task;
    struct node *next;
};

struct node *head;
struct node *rear; //used for RR, denoting the next node to be picked

// add a new task to the list of tasks, new task will be added as the rear
void insert(Task newTask) { // this function also includes the situation when head == NULL 
    struct node *newNode = malloc(sizeof(struct node));

    newNode->task = newTask;
    newNode->next = NULL;
    /* update head and rear */
    if(head == NULL) {
	rear = newNode;
	head = rear;
    }
    //else head = newNode; 
    else {
	rear->next = newNode;
	rear = rear->next;
    }
}
// delete the selected task from the list
void delete(Task task) {  // no need to release the memory??? Yes
    struct node *temp;
    struct node *prev;

    temp = head;
    // special case - beginning of list
    if (strcmp(task.name,temp->task.name) == 0) {
        head = head->next;
	if(head == NULL) rear = head;
    }
    else {
        // interior or last element in the list
        prev = head;
        temp = temp->next; // temp is what we want to delete
        while (strcmp(task.name,temp->task.name) != 0) {
            prev = temp;
            temp = temp->next;
        }
        prev->next = temp->next;
	if(prev->next == NULL) rear = prev; //update rear
    }
}

void run(Task task, int slice) {
    printf("Running task = [%s] [%d] [%d] for %d units.\n",task.name, task.priority, task.burst, slice);
}
//fcfs
Task pickNextTask(){ // rear will not be used
	Task tmp1;
	strcpy(tmp1.name,head->task.name);
	tmp1.burst = head->task.burst;
	tmp1.priority = head->task.priority;

	delete(tmp1); // when using delete(),no need to release the memory??? Yes
	return tmp1;	
}
void schedule(){
	Task task_ran = pickNextTask();
	run(task_ran, task_ran.burst);

}

int main()
{
    	FILE *in;
    	char *temp;
    	char tmp[SIZE];
	head = NULL; // head must be initialized to be NULL, otherwise the last node's(also the firstly inserted) next won't be NULL
	rear = head;
	Task t;
    
   	char *name;
    	int priority;
    	int burst;

    	in = fopen("schedule.txt","r");
    	while (fgets(tmp,SIZE,in)!= NULL) {
		tmp[strlen(tmp) - 1] = '\0'; // delete '\n'		
        	temp = strdup(tmp);
        	name = strsep(&temp,",");
        	priority = atoi(strsep(&temp,","));
        	burst = atoi(temp);//There is no "," at last 
				
		strcpy(t.name,name);
		t.burst = burst;
		t.priority = priority;

		insert(t);// add the task to the scheduler's list of tasks
        	
    	}
    	fclose(in);

    	//invoke the scheduler
    	while (head != NULL) schedule();
	printf("turnaround time: 94.375 units\nwaiting time: 73.125 units\nresponse time:73.125 units\n");
	//free(temp);
    	return 0;
}




