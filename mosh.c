/*

CSC 360 Assignment 2

Cameron Long V00748439

mosh.c

Mini-shell program

	

/* Include Delcarations*/
#include <stdio.h>
#include <stdlib.h>                                                                            
#include <unistd.h>                                                                           
#include <string.h>                                                                           
#include <sys/types.h>                                                                     
#include <signal.h>                                                                            
#include <sys/wait.h>                                                                        
#include <fcntl.h>                                                                               
#include <termios.h>  
#include <time.h>

/* Global Variables*/
char PATH[200];
const char delimiter[3] = "++";
const char PATHDELIMITER[2] = ":";
char* token;
char* time_string;
/*Storage for PATH*/
char path1[30];
char path2[30];
char path3[30];
char path4[30];
char path5[30];
char path6[30];
char path7[40];
char path8[40];
char path9[30];
/*getting hostname and other semantics*/
char* cmdtoken;
char newcommand[40];
char hostname[20];
static char* curDir;
static char uinput = '\0';
char buffer[80];
static int bufferchar = 5;
static char *cmdlnArgs[5];
static int cmdArglength = 0;

#define STDIN 1
#define STDOUT 2


#define PROCESS_ID 1
#define JOB_ID 2
#define J_STATUS 3
#define FOREGROUND 'F'
#define BACKGROUND 'B'

static int activejobs = 0;

/*struct for storing job data, like pid and the like when forked*/
typedef struct job {

	int id;
	char* name;
	pid_t pid;
	int status;
	char* descriptor;
	struct job *next;

}struct_job;

static struct_job* jobs_list = NULL;
/*pid of the terimnal*/
static pid_t moshpid; 
/* "group pid of the terminal, used to resume control*/
static pid_t moshpgid;
static int moshterm, interactiveflag;
static struct termios moshmodes;


struct_job* getJob(int searchVal, int parameter){

usleep(10000);
        struct_job* job = jobs_list;
        switch (parameter) {
        case PROCESS_ID:
                while (job != NULL) {
                        if (job->pid == searchVal)
                                return job;
                        else
                                job = job->next;
                }
                break;
        case JOB_ID:
                while (job != NULL) {
                        if (job->id == searchVal)
                                return job;
                        else
                                job = job->next;
                }
                break;
        case J_STATUS:
                while (job != NULL) {
                        if (job->status == searchVal)
                                return job;
                        else
                                job = job->next;
                }
                break;
        default:
                return NULL;
                break;
        }
        return NULL;
}

/*Deletes the job when its finished*/
struct_job* delJob(struct_job* job)
{
        usleep(10000);
        if (jobs_list == NULL)
                return NULL;
        struct_job* currentJob;
        struct_job* beforeCurrentJob;

        currentJob = jobs_list->next;
        beforeCurrentJob = jobs_list;

        if (beforeCurrentJob->pid == job->pid) {

                beforeCurrentJob = beforeCurrentJob->next;
                activejobs--;
                return currentJob;
        }

        while (currentJob != NULL) {
                if (currentJob->pid == job->pid) {
                        activejobs--;
                        beforeCurrentJob->next = currentJob->next;
                }
                beforeCurrentJob = currentJob;
                currentJob = currentJob->next;
        }
        return jobs_list;
}
/*The main signal handler of the program*/

/* will recieve "done" flags from background processes and print the data*/
void signalHandler_child(int p)
{
        pid_t pid;
        int terminationStatus;
        pid = waitpid(-1, &terminationStatus, WUNTRACED | WNOHANG); 
        if (pid > 0) {                                                                        
                struct_job* job = getJob(pid,PROCESS_ID);                   
                if (job == NULL)
                        return;
                if (WIFEXITED(terminationStatus)) {                                                    
                        if (job->status == BACKGROUND) { 
				time_t current_time;
				current_time = time(NULL);
				time_string = ctime(&current_time);                            
                                printf("\n[%d]+ Done : %s, Time : %s  Arguements: %s %s %s %s\n", (int)job->pid, job->name, time_string, cmdlnArgs[1], cmdlnArgs[2],cmdlnArgs[3],cmdlnArgs[4]); 
                                jobs_list = delJob(job);                                                
                        }
                
                } else {
                        if (job->status == BACKGROUND) {                          
                                jobs_list = delJob(job);
                        }
                }
		/*set the terminal back to foreground*/
                tcsetpgrp(moshterm, moshpgid);
        }
}

/*retrieve the PATH variable from the computer*/
void getPATH(){
int count = 0;

strcpy(PATH,getenv("PATH"));


token = strtok(PATH, PATHDELIMITER);
while(token != NULL){
token = strtok(NULL, PATHDELIMITER);



/*Copy each possible path into a global string*/
if (token !=NULL){
switch (count){
	case 0:
	strncpy(path1, token,strlen(token));
	count++;
	break;
	case 1:
	strncpy(path2, token,strlen(token));
	count++;
	break;
	case 2:
	strncpy(path3, token,strlen(token));
	count++;
	break;
	case 3:
	strncpy(path4, token,strlen(token));
	count++;
	break;
	case 4:
	strncpy(path5, token,strlen(token));
	count++;
	break;
	case 5:
	strncpy(path6, token,strlen(token));
	count++;
	break;
	case 6:
	strncpy(path7, token,strlen(token));
	count++;
	break;
	case 7:
	strncpy(path8, token,strlen(token));
	count++;
	break;
	case 8:
	
	strncpy(path9, token,strlen(token));
	count++;
	break;

	default:
	break;
}
}
}
}
void startScreen()
{
/*first screen of the shell
*prints host computer name and date

* also gives pid of the master processes or terminal
*
*/

printf("\n");
hostname[19] = '\0';
gethostname(hostname, 20);
printf("Welcome to Mini-shell\n");
printf("-----------------------------\n");
time_t current_time;
current_time = time(NULL);
time_string = ctime(&current_time);
(void)printf("Current Time: %s", time_string);
printf("Hosted On: %s\n", hostname);

printf("Shell process ID : %d\n" , (int)moshpid);

/*print found paths*/
printf("\nUsable PATHs-------------------------------\n\n");
getPATH();
}
/*wrapper for chdir*/
void changeDirectory()
{
        if (cmdlnArgs[1] == NULL) {
                chdir(getenv("HOME"));                                                      
        } else {
                if (chdir(cmdlnArgs[1]) == -1) {                                 
                        printf(" %s: no such directory\n", cmdlnArgs[1]);
                }
        }
}

/*pseudo prompt for the user that shows the directory*/
void minishellcmdPrompt()
{

printf("%s /  %s Active Jobs: %d> ", getenv("LOGNAME") , getcwd(curDir, 1024), activejobs);

}
/*returns the active jobs and statuses*/
/*only when running in background*/
void cmdall()
{

printf("Active Jobs\n");
printf("%s | %s | %s | %s | %s |\n" , "Job :" , "Name" , "pid" , "descriptor", "status");
struct_job *job = jobs_list;
 if (job == NULL) {
                printf("| %s %62s |\n", "No Jobs.", "");
        } else {
                while (job != NULL) {
                        printf("|  %7d | %30s | %5d | %10s | %6c |\n", job->id, job->name,
                               job->pid, job->descriptor, job->status);
                        job = job->next;
                }
        }
	 


}
/*destroys all the data in the args and buffer*/
/*ABSOLUTELY necessary in order to parse each command effectively*/
void clear()
{
        while (cmdArglength != 0) {
                cmdlnArgs[cmdArglength] = NULL;                                 
                cmdArglength--;                                                                       
        }
        bufferchar = 0;                                                                          
}


/*inserts a new job into the jobs_list linked list*/

struct_job* insertJob(pid_t pid, pid_t pgid, char* name, char* descriptor,
                 int status)
{
        usleep(10000);
        struct_job *newJob = malloc(sizeof(struct_job));

        newJob->name = (char*) malloc(sizeof(name));
        newJob->name = strcpy(newJob->name, name);
        newJob->pid = pid;
        newJob->status = status;
        newJob->descriptor = (char*) malloc(sizeof(descriptor));
        newJob->descriptor = strcpy(newJob->descriptor, descriptor);
        newJob->next = NULL;

        if (jobs_list == NULL) {
                activejobs++;
                newJob->id = activejobs;
                return newJob;
        } else {
                struct_job *auxNode = jobs_list;
                while (auxNode->next != NULL) {
                        auxNode = auxNode->next;
                }
                newJob->id = auxNode->id + 1;
                auxNode->next = newJob;
                activejobs++;
                return jobs_list;
        }
}





/**
 * kills a Job given its number
 */
void killJob(int jobId)
{
        struct_job *job = getJob(jobId,JOB_ID);                                   
        kill(job->pid, SIGKILL);                                                               
}
void waitJob(struct_job* job)
{
        int terminationStatus;
        while (waitpid(job->pid, &terminationStatus, WNOHANG) == 0) {      
               }
        jobs_list = delJob(job);                                            
}

void start()
{
	/*generate the she;l's terminal information*/
	moshpid = getpid();
	moshterm = STDIN_FILENO;
	interactiveflag = isatty(moshterm);
	
	if (interactiveflag)
		{
		while (tcgetpgrp(moshterm) != (moshpgid = getpgrp()))
			kill(moshpid, SIGTTIN);
			
		
		/*set the signals to thier defaults*/
		signal(SIGQUIT, SIG_IGN);
                signal(SIGTTOU, SIG_IGN);
                signal(SIGTTIN, SIG_IGN);
                signal(SIGTSTP, SIG_IGN);
                signal(SIGINT, SIG_IGN);
                signal(SIGCHLD, &signalHandler_child);

	setpgid(moshpid, moshpid);
	moshpgid = getpgrp();
	if (moshpid != moshpgid)
		{
		printf("Error: vould not make shell thREaD Leader");
		exit(EXIT_FAILURE);
		}
		if (tcsetpgrp(moshterm, moshpid) == -1)
			{
			tcgetattr(moshterm, &moshmodes);

			}
		
		curDir = (char*)calloc(1024, sizeof(char));
	}else {

	printf("Failure");
	exit(EXIT_FAILURE);

}

}
void executeCommand(char *command[], char *file, int newDescriptor,
                    int executionMode)
{
	/*gets here in command was not a built-in command*/
	/*Looks through all the paths for the command*/

        int commandDescriptor;
        /**
         *  Set the STDIN/STDOUT channels of the new process.
         */

	
        if (newDescriptor == STDIN) {
                commandDescriptor = open(file, O_RDONLY, 0600);                                        
                dup2(commandDescriptor, STDIN_FILENO);
                close(commandDescriptor);
        }
        if (newDescriptor == STDOUT) {
                commandDescriptor = open(file, O_CREAT | O_TRUNC | O_WRONLY, 0600); 
                dup2(commandDescriptor, STDOUT_FILENO);
                close(commandDescriptor);
        }

	strcpy(newcommand, path1);
	strcat(newcommand, "/");
	strcat(newcommand, command[0]);
	
	if (execv(newcommand, command) == -1)
		{
		memset(&newcommand, 0 ,40);
		}
	strcpy(newcommand, path2);
	strcat(newcommand, "/");
	strcat(newcommand, command[0]);
	
	
	if (execv(newcommand, command) == -1)
		{
		memset(&newcommand, 0 ,40);
		}
	strcpy(newcommand, path3);
	strcat(newcommand, "/");
	strcat(newcommand, command[0]);
	
	if (execv(newcommand, command) == -1)
		{
		memset(&newcommand, 0 ,40);
		}
	strcpy(newcommand, path4);
	strcat(newcommand, "/");
	strcat(newcommand, command[0]);
	
	
	if (execv(newcommand, command) == -1)
		{
		memset(&newcommand, 0 ,40);
		}
	strcpy(newcommand, path5);
	strcat(newcommand, "/");
	strcat(newcommand, command[0]);
	
	if (execv(newcommand, command) == -1)
		{
		memset(&newcommand, 0 ,40);
		}
	strcpy(newcommand, path6);
	strcat(newcommand, "/");
	strcat(newcommand, command[0]);
	
	
	if (execv(newcommand, command) == -1)
		{
		memset(&newcommand, 0 ,40);
		}
	strcpy(newcommand, path7);
	strcat(newcommand, "/");
	strcat(newcommand, command[0]);
	
	if (execv(newcommand, command) == -1)
		{
		memset(&newcommand, 0 ,40);
		}
	strcpy(newcommand, path8);
	strcat(newcommand, "/");
	strcat(newcommand, command[0]);
	if (execv(newcommand, command) == -1)
		{
		memset(&newcommand, 0 ,40);
		}
	strcpy(newcommand, path9);
	strcat(newcommand, "/");
	strcat(newcommand, command[0]);
	
	if (execv(newcommand, command) == -1)
		{
		memset(&newcommand, 0 ,40);
		}
  	else{
		printf("The command was not found\n");
		}

}
void putJobForeground(struct_job* job, int continueJob)
{
        job->status = FOREGROUND;                                                   
        tcsetpgrp(moshterm, job->pid);                                 
        if (continueJob) {                                                                        
                if (kill(-job->pid, SIGCONT) < 0)                                           
                        perror("kill (SIGCONT)");
        }

        waitJob(job);                                                                                 
        tcsetpgrp(moshterm, moshpgid);                              
}

void putJobBackground(struct_job* job, int continueJob)
{
        if (job == NULL)
                return;
        if (continueJob)                        
                if (kill(-job->pid, SIGCONT) < 0)
                        perror("kill (SIGCONT)");

        tcsetpgrp(moshterm, moshpgid);                             
}
/*Main launched of the program, gets all the args and such in order to be launched accordingly*/
void launchjob(char *command[], char *file, int newDescriptor, int executionMode) {
        pid_t pid;
        pid = fork();
        switch (pid) {
        case -1:
                perror("fork failed");
                exit(EXIT_FAILURE);
                break;
        case 0:
                /**
                 *  we set the handling for job control signals back to the default.
                 */
                signal(SIGINT, SIG_DFL);
                signal(SIGQUIT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
                signal(SIGCHLD, &signalHandler_child);
                signal(SIGTTIN, SIG_DFL);
                usleep(20000);                                                             
                setpgrp();                                                                                     
                if (executionMode == FOREGROUND)
                        tcsetpgrp(moshterm, getpid());                                           
                if (executionMode == BACKGROUND)
                        printf("[%d] %d\n", activejobs, (int) getpid());              

                executeCommand(command, file, newDescriptor, executionMode);

                exit(EXIT_SUCCESS);
                break;
        default:
                setpgid(pid, pid);                                                                        
                
                jobs_list = insertJob(pid, pid, *(command), file, (int) executionMode); 

                struct_job* job = getJob(pid, PROCESS_ID);                             

                if (executionMode == FOREGROUND) {
                        putJobForeground(job, 0);                                              
                }
                if (executionMode == BACKGROUND)
                        putJobBackground(job, 0);                                             
                break;
        }
}
/*will first check these commands and run them accordingly*/
int parseCommands()
{
	if (strcmp("exit", cmdlnArgs[0]) == 0 ){
		exit(EXIT_SUCCESS);
	}

	if (strcmp("chdir", cmdlnArgs[0]) == 0){
		changeDirectory();
		return 1;
	}
	if (strcmp("toback", cmdlnArgs[0]) == 0){
		if(cmdlnArgs[1] == NULL){
			return 0;
			}
		else {
		launchjob(cmdlnArgs + 1, "STANDARD", 0 , BACKGROUND);
		return 1;
		}
	}
	if(strcmp("dirlist", cmdlnArgs[0]) == 0){
	
		char* ls = "ls";
		strcpy(cmdlnArgs[0], ls);
		launchjob(cmdlnArgs, "STANDARD", 0, FOREGROUND);
		return 1;

	}
		
	
	if (strcmp("cmdall", cmdlnArgs[0]) == 0)
		{
		cmdall();
		return 1;
		}
	if (strcmp("cmdkill", cmdlnArgs[0]) == 0){
		if(cmdlnArgs[0] == NULL)
			return 0;
		kill(atoi(cmdlnArgs[1]), SIGKILL);
		return 1;
		}

	return 0;
}
/*Parse commands will return 0 if there is no built in command associated with it

moves to launch job, which will pass to execute, to find a program to run*/
void readCommand()
{
	
	if ((parseCommands(cmdlnArgs[0])) == 0 ) {

		launchjob(cmdlnArgs, "STANDARD", 0 , FOREGROUND);
}

}
/*Our buffer is where the commands are read from

This function puts data into args
*/
void populateCommand()
{
        char* bufferPointer;                                                                  
        bufferPointer = strtok(buffer, " ");
        while (bufferPointer != NULL) {                                               
                cmdlnArgs[cmdArglength] = bufferPointer;                   
                bufferPointer = strtok(NULL, " ");
                cmdArglength++;
        }
}
/*Checks for multiple commands and (hopefully) parses them accordingly

the commands will be run in sequence as they are found*/
void getText()
{
        clear();                                                                     
        while ((uinput != '\n') && (bufferchar < 80)) {
                buffer[bufferchar++] = uinput;
                uinput = getchar();
        }
        buffer[bufferchar] = '\0';
	
	if(strstr(buffer, delimiter) == NULL){
	printf("%s\n" , buffer);                                                   
        populateCommand();
	readCommand();
	}
	else{
	printf("entered Else\n");
	cmdtoken = strstr(buffer, delimiter);
	printf("%s\n", cmdtoken);
	
	
	printf("%s\n", cmdtoken);
	
	buffer[strlen(buffer)-strlen(cmdtoken)] = '\0';
	printf("%s\n", buffer);
	cmdtoken = cmdtoken +2;
	populateCommand();
	readCommand();
	
		if(strstr(cmdtoken, delimiter) == NULL)
			{
				printf("Made it here\n");
				clear();
				strncpy(buffer, cmdtoken, strlen(cmdtoken));
				buffer[strlen(cmdtoken)] = '\0';
				printf("%s\n", buffer);
				populateCommand();
				readCommand();
				memset(&buffer, 0 , 80);
				
			}
		else {
			
			char temp[80];	
			clear();
			strncpy(temp, cmdtoken, strlen(cmdtoken));			
			cmdtoken = strstr(cmdtoken, delimiter);
			temp[strlen(temp) - strlen(cmdtoken)] = '\0';
			
			
			strncpy(buffer, temp, strlen(temp));
		
			buffer[strlen(temp)] = '\0'; 
			populateCommand();
			readCommand();
			clear();
			
			cmdtoken = cmdtoken +2;
			
			printf("%s\n", temp);
			strncpy(buffer, cmdtoken, strlen(cmdtoken));
			
			buffer[strlen(cmdtoken)] = '\0';
			populateCommand();
			readCommand();
			clear();
			
			
			memset(&buffer, 0 , 80);
		
			}
	}
	

}
/*Always loops back to main*/
int main(int argc, char **argv, char **envp)
{
        start();
        startScreen();
        minishellcmdPrompt();                                                                         
        while (1) {
                uinput = getchar();
                switch (uinput) {
                case '\n':                                                                               
                        minishellcmdPrompt();                                                                     
                        break;
                default:
                        getText();                                                                     
                                                                           
                       minishellcmdPrompt();                                                                    
                        break;
                }
        }
        printf("\n");
        return 0;
}








