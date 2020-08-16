#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>  //call exit
#include <sys/wait.h> //call wait
#include <fcntl.h> //open's parameters
#define MAX_LINE 80 /* 80 chars per line, per command */
#define READ_END  0
#define WRITE_END  1

int main(void)
{
	char *args[MAX_LINE/2 + 1];	// command line (of 80) has max of 40 arguments
	char *args1[MAX_LINE/2 + 1]; 	// used for pipe command
    	char arg[MAX_LINE];
	char buffer[MAX_LINE] = {'\0'}; // the most recent command history buffer should be initialized
	int should_run = 1;
	int background,i,i1,fd[2],pfd[2],tmp1,tmp2,tmp3; // i is the number of valid tokens in each command ,tmp1 and tmp2 is used for saving the original description
	int pi; // pi shows that whether there is the command pipe
	pid_t pid,ppid;
		
    	while (should_run){   
        	printf("osh>");
        	fflush(stdout);
		
		background = 0;
		i = 0; 
		pi = 0;
		tmp1 = dup(STDOUT_FILENO);
		tmp2 = dup(STDIN_FILENO);
		/* read the command */
		fgets(arg,MAX_LINE,stdin);
		if(arg[0] == '\n') continue;  //no commands
		if(arg[strlen(arg)-1] == '\n') arg[strlen(arg)-1] = '\0'; //fgets can take in the last '\n'
		
		if(strcmp("exit",arg) == 0){
			should_run = 0;
			break;
		}
		if(strcmp("!!",arg) == 0){
			if(buffer[0] == '\0') {printf("No commands in history.\n"); continue;}
			else strcpy(arg,buffer);
		}
		strcpy(buffer,arg);//update buffer
		
		/* divide the command into tokens */ 
		args[0] = strtok(arg," ");
		while(args[i] != NULL){
			i++;
			args[i] = strtok(NULL," "); 
		}
		/* args[i] is NULL, args[i-1] is the last vaid token */
		if(strcmp(args[i-1],"&") == 0){
			background = 1;	
			args[i-1] = NULL;
			i--; //update i	
		}
        	/* input and output redirection */
		/* if should redirect, according to the command form, "<" or ">" must appear in args[i-2], and the file's name msut be in args[i-1] */
		if(strcmp(args[i-2],"<") == 0 || strcmp(args[i-2],">") == 0){
			fd[0] = open(args[i-1],O_RDWR|O_CREAT,0644); //create or open this file

			if(fd[0]==-1){ // open file error
                		printf("open file error:%m\n");
                		exit(-1);
        		}

			if(strcmp(args[i-2],"<") == 0) fd[1]=dup2(fd[0],0); 
                        /*stdin's file descripter is 0. */
			/*???:why is "dup2(0,f[0])" not correct? explanation: forms are 0 or 1 is in the right parameter*/
			else if(strcmp(args[i-2],">") == 0) fd[1]=dup2(fd[0],1);//stdout's file descripter is 1

			if(fd[1]==-1){ //redirect error
                		printf("redirect error:%m\n");
                		exit(-1);
       			}

			args[i-1] = args[i-2] = NULL;
			i=i-2; //update i
		}		
		/* pipe */
		/* devide the pipe command into two parts */
		for(tmp3 = 0;tmp3 < i;++tmp3)
			if(strcmp(args[tmp3],"|") == 0) break;
		if(tmp3 != i){ // the command contains "|"
			pi = 1; // there is command pipe
			i1 = 0;	
			while(tmp3 < i){
				args1[i1++] = args[++tmp3]; // get the command args1
				args[tmp3-1] = NULL; //by the way update the command args 
			}
			i1--; // i1 should substract the NULL token		
		}
		
		
		pid = fork();
		if(pid < 0){ //error eccured
			fprintf(stderr, "Fork Failed\n");
			return 1;
		}
		if(pid == 0){ //child process
			if (pi == 1){
				if (pipe(pfd) == -1) { // create the pipe
					fprintf(stderr,"Pipe failed");
					return 1;
				}

				ppid = fork();

				if (ppid < 0) {
					fprintf(stderr, "Fork failed");
					return 1;
				}
				if(ppid == 0) { /* child process */
					close(pfd[WRITE_END]);/* close the unused end of the pipe */
					dup2(pfd[READ_END],0);

					/*i1++;
					args1[i1-1] = "pfd[READ_END]";
					args[i1] = NULL;*/

					execvp(args1[0],args1);
					close(pfd[READ_END]);/* close the read end of the pipe */
					dup2(tmp2,0/*STDIN_FILENO*/); // recover the direction
					exit(0);
				}
				if (ppid > 0) { /* parent process */
					printf("parent waits...\n");
					close(pfd[READ_END]);/* close the unused end of the pipe */
					dup2(pfd[WRITE_END],1);
					execvp(args[0],args);
					close(pfd[WRITE_END]);/* close the write end of the pipe */
					dup2(tmp1,1/*STDOUT_FILENO*/); // recover the direction
					while(pid != wait(NULL)); //jump out when wait returns exactly the pid of the child process
					printf("parent continues...\n");
				}
			}
			else execvp(args[0],args);
			
			exit(0);
		}
		else if(pid > 0){ //parent process
			if(background == 0){
				printf("parent waits...\n");
				while(pid != wait(NULL)); //jump out when wait returns exactly the pid of the child process
				printf("parent continues...\n");
			}
			else printf("run concurrently\n");

		}
		dup2(tmp1,1/*STDOUT_FILENO*/);
		dup2(tmp2,0/*STDIN_FILENO*/); //recover the direction
		close(fd[0]);
		
        	/**
         	 * After reading user input, the steps are:
         	 * (1) fork a child process
         	 * (2) the child process will invoke execvp()
         	 * (3) if command includes &, parent and child will run concurrently
         	 */
    	}
    
	return 0;
}
