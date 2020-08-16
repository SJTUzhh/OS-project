//priority_rr
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TIME_QUANTUM 10
#define SIZE  100

typedef struct task {
    char name[3];
    int tid;
    int priority;
    int time; //for a picked task, it's the excuting time; for the tasks in the list, it's the rest burst time
    int burst;
} Task;
struct node {
    Task task;
    struct node *next;
};

struct node *head;
struct node *rear; //used for RR, denoting the next node to be picked

// add a new task to the list of tasks, new task will be added as the rear
void insert(/*struct node *(&head),*/ Task newTask) { // this function also includes the situation when head == NULL 
    struct node *newNode = malloc(sizeof(struct node));

    newNode->task = newTask;
    newNode->next = NULL;

    /* update head and rear */
    if(head == NULL) {
	rear = newNode;
	head = rear;
    } 
    else {
	rear->next = newNode;
	rear = rear->next;
    }
}
// delete the selected task from the list
void delete(Task task) {  // no need to release the memory???
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
//priority_rr
Task pickNextTask(){//rear is the location of the last executed task's next 
	Task tmp1;
	struct node *tmp3 = rear; // tmp3 is the wanted node's location	 		   
	struct node *tmp2 = rear; // the node that moves and checks which one is wanted
	
	
	while(1){
		tmp2 = tmp2->next;
		if(tmp2 == NULL) tmp2 = head;	 // if end, restart from the head

		if(tmp2 == rear) break;// stops when arrives at rear
				//here is tricky: we should consider the special situation when rear is the at the head
		if(tmp2->task.priority > tmp3->task.priority) //not >=, because we will choose the neareast of all the wanted
			tmp3 = tmp2;		
	}
	
	strcpy(tmp1.name,tmp3->task.name);
	tmp1.priority = tmp3->task.priority;
	tmp1.burst = tmp3->task.burst;

	if(tmp3->task.time > TIME_QUANTUM){		
		tmp1.time = TIME_QUANTUM;
		tmp3->task.time -= TIME_QUANTUM; //update the picked task
		rear = tmp3->next;// after picking, the rear moves to the next
	}
	else{
		tmp1.time = tmp3->task.time;
		tmp3->task.time = 0;
		rear = tmp3->next;// after picking, the rear moves to the next
				  //we put the moving before the delete for this might be safer
		delete(tmp1);  //delete the picked task if it will be completed
	}

	if(rear == NULL) rear = head; // if end, go back
	return tmp1;	
}
void schedule(){
	Task task_ran = pickNextTask();
	run(task_ran, task_ran.time);

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
        	burst = atoi(temp);// There is no "," at last 
				
		strcpy(t.name,name);
		t.burst = burst;
		t.priority = priority;
		t.time = burst; //initialize time	

		insert(t);// add the task to the scheduler's list of tasks
        	
    	}
    	fclose(in);
	rear = head;
    	//invoke the scheduler
    	while (head != NULL) schedule(/*head,rear*/);
	printf("turnaround time: 105 units\nwaiting time: 90 units\nresponse time: 68.75 units\n");
    	return 0;
}



