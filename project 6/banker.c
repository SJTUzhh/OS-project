#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
#define MAX_LINE 80
#define true 1
#define false 0
int available[NUMBER_OF_RESOURCES];/* the available amount of each resource */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];/*the maximum demand of each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];/* the amount currently allocated to each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];/* the remaining need of each customer */

int is_not_greater_than(int n[],int w[],int s1,int s2){ //s1 and s2 should be equal; return 1 when true
	int flag = true;
	for(int i = 0; i < s1; ++i){
		if(n[i] > w[i]) flag = false;
	}
	return flag;			
}
int isSafe(){  // this function won't modify the value of the four matrix above; return 1 when true
	int tmp = true;
	int work[NUMBER_OF_RESOURCES];
	int finish[NUMBER_OF_CUSTOMERS];
	for(int i = 0; i < NUMBER_OF_RESOURCES; ++i) work[i] = available[i];
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; ++i) finish[i] = false;
	int d;
	do{	
		d = false;
		for(int i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
			if(finish[i] == false && is_not_greater_than(need[i],work,NUMBER_OF_RESOURCES,NUMBER_OF_RESOURCES)){
				for(int j = 0; j < NUMBER_OF_RESOURCES; ++j) work[j] += allocation[i][j];
				finish[i] = true;
				d = true;
			}
		}	
	}while(d == true);
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
		if(finish[i] == false) tmp = false;
	}
	return tmp;
}
int request_resources(int customer_num, int request[]){ //returns 0 when succcessful and -1 when unsuccessful
	if(!is_not_greater_than(request,need[customer_num],NUMBER_OF_RESOURCES,NUMBER_OF_RESOURCES)){
		printf("Error: Request is more than need!\n");
		return -1;
	}
	if(!is_not_greater_than(request,available,NUMBER_OF_RESOURCES,NUMBER_OF_RESOURCES)){
		printf("Request should wait\n");
		return -1;	
	}
	for(int k = 0; k < NUMBER_OF_RESOURCES; ++k){
		available[k] -= request[k];
		allocation[customer_num][k] += request[k];
		need[customer_num][k] -= request[k];
	}
	if(isSafe()) { printf("Request is satisfied\n"); return 0; }
	else {
		for(int k = 0; k < NUMBER_OF_RESOURCES; ++k){
			available[k] += request[k];
			allocation[customer_num][k] -= request[k];
			need[customer_num][k] += request[k];
		}
		printf("Request is not safe\n");
		return -1;
	}
				

} 
void release_resources(int customer_num, int release[]){
	if(is_not_greater_than(release,allocation[customer_num],NUMBER_OF_RESOURCES,NUMBER_OF_RESOURCES)){
		for(int k = 0; k < NUMBER_OF_RESOURCES; ++k){
			available[k] += release[k];
			allocation[customer_num][k] -= release[k];
			need[customer_num][k] += release[k];
		}
		printf("Released successfully\n");
	}
	else printf("Error:to be released is more than allocated!\n");
}	
void print() { // print the four matrix
	printf("Available\n");
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
		printf("%d ", available[i]);
	}
	printf("\nMax ");
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
		printf("\n");
		for (int j = 0; j < NUMBER_OF_RESOURCES; j++) 
			printf("%d ", maximum[i][j]);
	}
	printf("\nAllocation ");
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
		printf("\n");
		for (int j = 0; j < NUMBER_OF_RESOURCES; j++) 
			printf("%d ", allocation[i][j]);
	}
	printf("\nNeed ");
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++){
		printf("\n");
		for (int j = 0; j < NUMBER_OF_RESOURCES; j++) 
			printf("%d ", need[i][j]);
	}
	printf("\n");
}

int main(int argc,char *argv[])
{
	FILE *in = NULL;	
	/* generate available */
	for(int h = 0; h < argc-1; h++){ 
		available[h] = atoi(argv[h+1]);
	}
	/* generate allocation */
	for(int h = 0; h < NUMBER_OF_CUSTOMERS; h++)
		for(int g = 0; g < NUMBER_OF_RESOURCES; g++)
			allocation[h][g] = 0;
	/* generate maximum */
	in = fopen("in.txt","r");
	char temp1[80];// the size of temp is set to be more than enough
	char *temp;
	temp = temp1;
	int row = 0;
	while (fgets(temp,80,in) != NULL) { // here fgets stops only when '\n' or EOF(return NULL)
		temp[strlen(temp) - 1] = '\0'; // delete '\n'
		int col;
		for(col = 0; col < NUMBER_OF_RESOURCES-1; col++)
			maximum[row][col] = atoi(strsep(&temp,","));  
		// here is tricky: the last token does not have ",", if call strseq(), error will occur, so I deal with this token seperately.
		maximum[row][col] = atoi(temp);		
		++row;
	}	
	fclose(in);
	in = NULL;
	/* generate need */
	for(int h = 0; h < NUMBER_OF_CUSTOMERS; h++)
		for(int g = 0; g < NUMBER_OF_RESOURCES; g++)
			need[h][g] = maximum[h][g] - allocation[h][g];
	/* response to user's commands */
	while(1){						
		char *args[MAX_LINE/2 + 1];	// command line (of 80) has max of 40 arguments
    		char arg[MAX_LINE];	
		printf("Your command(input quit to exit): ");
		fgets(arg,MAX_LINE,stdin);
		if(arg[0] == '\n') continue;  //no commands
		if(arg[strlen(arg)-1] == '\n') arg[strlen(arg)-1] = '\0'; // fgets can take in the last '\n'
		if(strcmp("quit",arg) == 0) break; // exit
		if(strcmp("*",arg) == 0) { print(); continue;} //print
		/* divide the command into tokens */ 
		int t = 0;
		args[0] = strtok(arg," ");
		while(args[t] != NULL){
			t++;
			args[t] = strtok(NULL," "); 
		}
		/* args[t] is NULL, args[t-1] is the last vaid token */
		if(strcmp("RQ",args[0]) == 0){
			int request[t-2];
			for(int i1 = 0;i1 < t-2; i1++) request[i1] = atoi(args[i1+2]);
			request_resources(atoi(args[1]),request);	
		}
		if(strcmp("RL",args[0]) == 0){
			int release[t-2];
			for(int i1 = 0;i1 < t-2; i1++) release[i1] = atoi(args[i1+2]);
			release_resources(atoi(args[1]), release);	
		}		
	}
	
	return 0;
}




















