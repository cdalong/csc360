/*
 * sched.c
 * 
 * Copyright 2014 Fisk <fisk@fisk-custom>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

/*
 * cpusched.c
 *
 * Skeleton code for solution to A#3, CSC 360
 * Spring 2014
 *
 * Prepared by: Michael Zastre (University of Victoria)
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define MAX_LINE_LENGTH 100

#define FCFS 0
#define PS   1
#define MLFQ 2
#define STRIDE 3

#define PRIORITY_LEVELS 4


/*
 * Stores raw event data from the input,
 * and has spots for per-task statistics.
 * You may want to modify this if you wish
 * to store other per-task statistics in
 * the same spot.
 */

typedef struct Task_t {
    int   arrival_time;
    float length;
    int   priority;

    float finish_time;
    int   schedulings;
    float cpu_cycles;
    int  stride;
    int   tickets;
    int pass;
    int upperbound;
    int lowerbound;
    
} task_t; 

typedef struct Queue
{
	int capacity;
	int size;
	int front;
	int rear;
	task_t *piority_task;
	int piority;
	int *elements;

}Queue;


Queue * createQueue(int maxElements){

Queue *Q;

Q = (Queue*)malloc(sizeof(task_t)*maxElements);
Q->size = 0;
Q->capacity = maxElements;
Q->front = 0;
Q->rear = -1;
Q->piority_task = NULL;
return Q;


}

void Dequeue(Queue *Q){

if(Q->size==0){

printf("Empty Queue/n");
return;

}

else {

Q->size--;
Q->front++;
if(Q->front==Q->capacity)

{
Q->front = 0;
}

}
return;


}

int front(Queue *Q){


	if(Q->size==0)
	{
	printf("Queue is Empty/n");
	exit(0);
	}
return Q->elements[Q->front];

}


void Enqueue(Queue *Q, task_t *task){

printf("Made it to Enqueue\n");

	if(Q->size == Q->capacity)
	{
	printf("Queue is full \n");

	}
	else{
		
		Q->size++;
		Q->rear = Q->rear +1;
		printf("Exited Else\n");
		if (Q->rear == Q->capacity)

		{
			Q->rear = 0;
			
		}
		printf("Enqueued %d\n", Q->rear);
	/*Q->piority_task[Q->rear] = *task;*/
	printf("Enqueued %d", task->priority);
	
}
return;
}

/*
 * Some function prototypes.
 */

void read_task_data(void);
void init_simulation_data(int);
void first_come_first_serve(void);
void stride_scheduling(int);
void priority_scheduling(void);
void mlfq_scheduling(int);
void run_simulation(int, int);
void compute_and_print_stats(void);
Queue *Pior1 = NULL;
Queue *Pior2 = NULL;	
Queue *Pior3 = NULL;	
Queue *Pior4 = NULL;	
int current_pass = 0;
/*
 * Some global vars.
 */
int     num_tasks = 0;
task_t *tasks = NULL;

int global_pass;
int global_tickets = 10000;
int global_stride;
int current_tick =0;
int task_counter;
int total_priority;
int count =0;

void compute_and_print_stats()
{
    int tasks_at_level[PRIORITY_LEVELS] = {0,};
    float response_at_level[PRIORITY_LEVELS] = {0.0, };
    int scheduling_events = 0;
    int i;

    for (i = 0; i < num_tasks; i++) {
        tasks_at_level[tasks[i].priority]++;
        response_at_level[tasks[i].priority] += 
            tasks[i].finish_time - (tasks[i].arrival_time * 1.0);
        scheduling_events += tasks[i].schedulings;

        printf("Task %3d: cpu time (%4.1f), response time (%4.1f), waiting (%4.1f), schedulings (%5d)\n",
            i, tasks[i].length,
            tasks[i].finish_time - tasks[i].arrival_time,
            tasks[i].finish_time - tasks[i].arrival_time - tasks[i].cpu_cycles,
            tasks[i].schedulings);
            
    }

    printf("\n");

    if (num_tasks > 0) {
        for (i = 0; i < PRIORITY_LEVELS; i++) {
            if (tasks_at_level[i] == 0) {
                response_at_level[i] = 0.0;
            } else {
                response_at_level[i] /= tasks_at_level[i];
            }
            printf("Priority level %d: average response time (%4.1f)\n",
                i, response_at_level[i]);
        }
    }

    printf ("Total number of scheduling events: %d\n", scheduling_events);
}

void read_task_data()
{
    int max_tasks = 2;
    int  in_task_num, in_task_arrival, in_task_priority;
    float in_task_length;
    

    assert( tasks == NULL );

    tasks = (task_t *)malloc(sizeof(task_t) * max_tasks);
    if (tasks == NULL) {
        fprintf(stderr, "error: malloc failure in read_task_data()\n");
        exit(1);
    }
   
    num_tasks = 0;

    /* Given the format of the input is strictly formatted,
     * we can used fscanf .
     */
    while (!feof(stdin)) {
        fscanf(stdin, "%d %d %f %d\n", &in_task_num,
            &in_task_arrival, &in_task_length, &in_task_priority);
        assert(num_tasks == in_task_num);
        tasks[num_tasks].arrival_time = in_task_arrival;
        tasks[num_tasks].length       = in_task_length;
        tasks[num_tasks].priority     = in_task_priority;

        num_tasks++;
        if (num_tasks >= max_tasks) {
            max_tasks *= 2;
            tasks = (task_t *)realloc(tasks, sizeof(task_t) * max_tasks);
            if (tasks == NULL) {
                fprintf(stderr, "error: malloc failure in read_task_data()\n");
                exit(1);
            } 
        }
    }
    task_counter = num_tasks;
}


void init_simulation_data(int algorithm)
{
    int i;

    for (i = 0; i < num_tasks; i++) {
        tasks[i].finish_time = 0.0;
        tasks[i].schedulings = 0;
        tasks[i].cpu_cycles = 0.0;
    }
}


void first_come_first_serve() 
{
    int current_task = 0;
    int current_tick = 0;

    for (;;) {
        current_tick++;

        if (current_task >= num_tasks) {
            break;
        }

        /*
         * Is there even a job here???
         */
        if (tasks[current_task].arrival_time > current_tick-1) {
            continue;
        }

        tasks[current_task].cpu_cycles += 1.0;
        
        if (tasks[current_task].cpu_cycles >= tasks[current_task].length) {
            float quantum_fragment = tasks[current_task].cpu_cycles -
                tasks[current_task].length;
            tasks[current_task].cpu_cycles = tasks[current_task].length;
            tasks[current_task].finish_time = current_tick - quantum_fragment;
            tasks[current_task].schedulings = 1;
            current_task++;
            if (current_task > num_tasks) {
                break;
            }
            tasks[current_task].cpu_cycles += quantum_fragment;
        }
    }
}


void stride_scheduling(int quantum)
{
	
  	
  	total_priority = 0;
  	global_tickets = 10000;
  	/*  divide tickets by piority */
  	
  	int i,p;
   	for ( p =0; p<num_tasks; p++){
		if (tasks[p].stride <= current_pass && tasks[p].finish_time == 0)	
	     total_priority += tasks[p].priority;
	    
			
       }
       
       if (total_priority == 0){
		   compute_and_print_stats();
		   exit(0);
	   }
  	for ( i =0; i<num_tasks; i++){ 
		
		 
		tasks[i].stride = tasks[i].priority * tasks[i].schedulings;
		 /*printf("Stride: %d %d \n", tasks[i].stride, current_pass);*/
		 
	
		 
		
		 if (global_tickets - tasks[i].tickets <=0){
			/* printf ("Not enough tickets \n");*/
		 break;
	 }
	 
	 
	 
			if (tasks[i].stride > current_pass || tasks[i].finish_time != 0)
			{
				tasks[i].tickets = 0;
		     
		     tasks[i].upperbound = 0;
			 global_tickets-= tasks[i].tickets;
			 tasks[i].lowerbound= 0;
			 current_pass++;
			}
		 
		 else{
			/* printf("Total Priority : %d\n", total_priority);*/
			 tasks[i].tickets = tasks[i].priority *(10000/total_priority);
		     
		     tasks[i].upperbound = global_tickets;
			 global_tickets-= tasks[i].tickets;
			 tasks[i].lowerbound= global_tickets+1;
			/* printf(" Upper; %d Lower  %d\n", tasks[i].upperbound, tasks[i].lowerbound);*/
		 
	 }
 }
		 
		 

		
	
		srand ( time(NULL) );
		int r = rand() % (10000 - global_tickets + 2);
		r = 10000 - r; 
		
		int j;
		 for (j =0; j<num_tasks; j++)
		 
		 {
			 
			 if (tasks[j].upperbound >= r && r >= tasks[j].lowerbound)
			 
			 {
				 /*task runs*/
				
				 
				 
				 if (tasks[j].length - quantum - tasks[j].cpu_cycles<=0 )
				 
				 {
					/* printf("Running task %d:\n\n" , j);*/
					 int  p = tasks[j].length- tasks[j].cpu_cycles;
				    tasks[j].cpu_cycles += p;
				    tasks[j].schedulings +=1; 
				    current_tick += p;
				    tasks[j].finish_time = current_tick;
				   /* printf("finish time %f current tick %d  schedulings %d\n" , tasks[j].finish_time, current_tick, tasks[j].schedulings);*/
				    
				   /* printf("TASK %d FINSIHED \\n\n\n", j);*/
				   current_pass++;
				   usleep(10000);
			     }
				 
				else {
					/*printf("Running task %d:\n" , j);*/
				   tasks[j].cpu_cycles += quantum;
				   current_tick+= quantum;
				   tasks[j].schedulings += 1; 
				  /* printf("Cycles %f current tick %d  schedulings %d\n" , tasks[j].cpu_cycles, current_tick, tasks[j].schedulings);*/
				   current_pass++;
				    usleep(10000);
				   
				 }
				 
			 }
			 
			 
		 }
		 		
		
		 stride_scheduling(quantum);
		
	
	  
		 printf("STRIDE SCHEDULING appears here\n");
    exit(1);
	
}

void priority_scheduling()
{

    int current_task = 0;
    int current_tick = 0;
	int highest_priority = 0;
	int lowest_time = 999;
	int i;
	
	
	for (i=0;i<num_tasks;i++){
		
		if (tasks[i].priority > highest_priority&&tasks[i].finish_time == 0){
			
			highest_priority = tasks[i].priority;
		}
		
	}
	for (i=0;i<num_tasks;i++){
		
		if(tasks[i].priority == highest_priority&&tasks[i].finish_time == 0){
			
			if (tasks[i].arrival_time <= lowest_time&&tasks[i].finish_time == 0){
				
					lowest_time = tasks[i].arrival_time;
			
		}
		}
		}
		
		
	
		
		for (i=0;i<num_tasks;i++){
			
			if(tasks[i].priority == highest_priority && lowest_time == tasks[i].arrival_time &&tasks[i].finish_time == 0){
				tasks[i].schedulings +=1;
				
				
				for(;;){
				current_tick +=1;
				tasks[i].cpu_cycles +=1;
				
			
				
					if (tasks[i].cpu_cycles >= tasks[i].length) {
            float quantum_fragment = tasks[i].cpu_cycles -
                tasks[current_task].length;
            tasks[i].cpu_cycles = tasks[i].length;
            tasks[i].finish_time = current_tick - quantum_fragment;
            tasks[i].schedulings = 1;
            i++;
            if (i > num_tasks) {
				
                break;
            }
            tasks[i].cpu_cycles += quantum_fragment;
            count++;
            if (count==num_tasks)
            {
				compute_and_print_stats();
				exit(0);
				
			}
            priority_scheduling();
        }
				
				
				}
				
			}
			
		}

}	
	
 

void roundRobin(){



	/* *split into quantums */
}
void runMLPQ(int quantum)

{


int current_task = 0;


while (Pior1->piority_task !=NULL){
	
	Pior1->piority_task[current_task].length -= quantum;
	
	if (Pior1->piority_task[current_task].length <=0){
		Dequeue(Pior1);
		
	}
	
	current_task ++;
}
current_task =0;
while (Pior2->piority_task !=NULL&&Pior1->piority_task !=NULL){
	
	Pior2->piority_task[current_task].length -= quantum;
	
	if (Pior2->piority_task[current_task].length <=0){
		Dequeue(Pior2);
		
	}
	
	current_task ++;
}
current_task =0;
while (Pior3->piority_task !=NULL&&Pior1->piority_task !=NULL&&Pior2->piority_task !=NULL){
	
	Pior3->piority_task[current_task].length -= quantum;
	
	if (Pior3->piority_task[current_task].length <=0){
		Dequeue(Pior3);
		
	}
	
	current_task ++;
}
current_task =0;
while (Pior4->piority_task !=NULL&&Pior1->piority_task !=NULL&&Pior3->piority_task !=NULL&&Pior2->piority_task !=NULL){
	
	Pior4->piority_task[current_task].length -= quantum;
	
	if (Pior4->piority_task[current_task].length <=0){
		Dequeue(Pior4);
		
	}
	
	current_task ++;
}






/*if piority queue 1 is not empty*/
/*run robin Queue 1*/

/*Continue with each queue*/


}

void sortintoQueues(int quantum){

/*

For each task

Enqueue task into each piority queue

*/
int current_task = 0;

while(&tasks[current_task]!= NULL){


  printf("MLFQ SCHEDULING appears here\n");
  printf("%d\n", tasks[current_task].priority);
  printf("%d\n", current_task);
  
  
  
switch(tasks[current_task].priority){
printf("Made it to switch/n");	
case 1:
	printf("Made it to Enqueue\n");	
	Enqueue(Pior1, &tasks[current_task]);
	current_task ++;
	break;
case 2:
	printf("Made it to Enqueue/n");
	Enqueue(Pior2, &tasks[current_task]);
	current_task ++;
	break;
case 3:
	printf("Made it to Enqueue/n");
	Enqueue(Pior3, &tasks[current_task]);
	current_task ++;
	break;
case 4:
	printf("Made it to Enqueue/n");
	Enqueue(Pior4, &tasks[current_task]);
	current_task ++;
	break;



}
}



runMLPQ(quantum);



}



void mlfq_scheduling(int quantum)
{
    printf("MLFQ SCHEDULING appears here\n");

	

	/* Queue each task into a different piority queue
		round robin each piority 
		cycle to next queue
		

        */	

	sortintoQueues(quantum);			



    exit(1);
}


void run_simulation(int algorithm, int quantum)
{
    switch(algorithm) {
        case STRIDE:
            stride_scheduling(quantum);
            break;
        case PS:
            priority_scheduling();
            break;
        case MLFQ:
            mlfq_scheduling(quantum);
            break;
        case FCFS:
        default:
            first_come_first_serve();
            break;
    }
}






int main(int argc, char *argv[])
{
	Pior1 = createQueue (10);
	Pior2 = createQueue (10);
	Pior3 = createQueue (10);
	Pior4 = createQueue (10);
	
    int i = 0;
    int algorithm = FCFS;
    int quantum = 1;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-q") == 0) {
            i++;
            quantum = atoi(argv[i]);
        } else if (strcmp(argv[i], "-a") == 0) {
            i++;
            if (strcmp(argv[i], "FCFS") == 0) {
                algorithm = FCFS;
            } else if (strcmp(argv[i], "PS") == 0) {
                algorithm = PS;
            } else if (strcmp(argv[i], "MLFQ") == 0) {
                algorithm = MLFQ;
            } else if (strcmp(argv[i], "STRIDE") == 0) {
                algorithm = STRIDE;
            }
        }
    }
         
    read_task_data();

    if (num_tasks == 0) {
        fprintf(stderr,"%s: no tasks for the simulation\n", argv[0]);
        exit(1);
    }

    init_simulation_data(algorithm);
    run_simulation(algorithm, quantum);
    compute_and_print_stats();

    exit(0);
}




