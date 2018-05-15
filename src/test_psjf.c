#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "util.h"
#include <signal.h>
#include <wait.h>
#include <assert.h>
//#include "sjf.h"
#include "scheduler.h"
/* for system call */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <time.h>
#include <sys/syscall.h>

// typedef struct {
//     char p_name[BUFF_SIZE];  
//     int ready_t;
//     int exec_t;
// 	pid_t pid;
// } Process;   //in utils.h


#ifndef PRINT_INTERVAL
#define PRINT_INTERVAL 100
#endif
//#define DEBUG 1

static int is_terminated = 0;
static pid_t exit_pid;
static int total_child = 0;
void psjf_sighandler(int signum){
    if (signum == SIGCHLD){
        is_terminated = 1;
        exit_pid = wait(NULL);
        //#ifdef DEBUG
        printf("Child terminated. pid = %d, child left = %d, terminated = %d\n", exit_pid, total_child-1, is_terminated);
        //#endif
    }
}

void psjf(Process* p,int N){
    //#ifndef DEBUG
    
    char *tag = "[Project1]";
    struct timespec ts_start;
    struct timespec ts_end;
    //#endif

    struct sigaction sa;
    sa.sa_handler = psjf_sighandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    assert(sigaction(SIGCHLD, &sa, NULL) != -1);
    // this part is to handle death of child process 

    struct sched_param sch_p;
	pid_t scheduler_pid = getpid();
    //#ifdef DEBUG
    printf("scheduler's pid = %d\n", scheduler_pid);
    //#endif
    sch_p.sched_priority = 3;
    assert(sched_setscheduler(scheduler_pid, SCHED_FIFO, &sch_p) != -1); 


    #ifdef DEBUG
    printf("Before sort\n");
    for(int i=0;i<N;i++){
    	printf("process info: ready_t=%d,exec_t=%d\n",p[i].ready_t,p[i].exec_t);
    }
    #endif
    QsortReady(p,N);//sort ready_t from small to large
    //#ifdef DEBUG
    printf("After sort\n");
    for(int i=0;i<N;i++){
    	printf("process info: ready_t=%d,exec_t=%d\n",p[i].ready_t,p[i].exec_t);
    }
//	#endif
    //this part is to sort for finding ready process

    pid_t fork_pid; //for the case dont need to create new pid
    total_child = N;
    Process currentP;//Now executing process 
    currentP.pid = -1;
    currentP.ready_t = -1;
    currentP.exec_t = -1; // smaller then zero when before the "currentP = first process" so that won't fork child
    Process* priority_heap = (Process*)malloc(N* sizeof(Process));
    int priority_heap_size = 0;
    int time_counter = 0;
    int exec_time_counter = 0 ; // to count the current child's exec_t 
    int ready_index = 0; //check if p[ready_index] ready
    while (total_child > 0){ // main parent loop
        printf("<debug> time counter at parent: %d,exec_time_counter: %d,total_child: %d,is_terminated: %d\n", time_counter,exec_time_counter,total_child,is_terminated);
    	//check counter if some other process ready,add to priority_heap, if ready_index=0 fork and run immediately
    	//while check whether p[ready_index] ready
    	while(ready_index<N && p[ready_index].ready_t==time_counter){
            //#ifdef DEBUG       
            printf("time counter at parent: %d\n\tReady process: ", time_counter);
            print(p[ready_index]);
            //#endif
    		priority_heap[priority_heap_size++] = p[ready_index];	//add this process to priority_heap
    		ToHeap(priority_heap,priority_heap_size);//clean but maybe slow QQ

    		ready_index++;
    	}

        if (p[0].ready_t == time_counter){
            is_terminated = 1;
            total_child++; //before the first process start, no need to --total_child in line 143, so need to print N there in first time
        }
    	if(is_terminated){//if execution time end, print counter
            
            is_terminated = 0;
            //#ifdef DEBUG
            printf("<terminate>time counter at parent: %d, currentP.exec_t: %d, total_child: %d\n", time_counter,currentP.exec_t,--total_child);
            //#endif
            if(priority_heap_size>0){//if no process running or ready, idle

                exec_time_counter = 0;
                currentP = priority_heap[0];
                syscall(335, &ts_start);
                if(currentP.ready_t<time_counter){//this currentP already created
                    fork_pid = fork();
                    if (fork_pid == 0){ // child
                        break;
                    } else if (fork_pid > 0) { // scheduler
                        printf("child recovered at %d. pid = %d\n",time_counter, currentP.pid);
                    }else{
                        printf("fork failed\n");
                    }
                }else{ // create new one , actually currentP.ready_t==time_counter
                    currentP.pid = fork(); // need to execute new preemptive process
                    if (currentP.pid == 0){ // child
                        break;
                    } else if (currentP.pid > 0) { // scheduler
                        printf("child created at %d. pid = %d\n",time_counter, currentP.pid);
                    }else{
                        printf("fork failed\n");
                    }
                }
                swap(&priority_heap[0],&priority_heap[--priority_heap_size]);//the last removed
                ToHeap(priority_heap,priority_heap_size);//clean but maybe slow QQ
                printf("Change control to: ");
                print(currentP);
                sch_p.sched_priority = 4;
                assert(sched_setscheduler(currentP.pid, SCHED_FIFO, &sch_p) != -1); // let the child run
            }
    		
    	}else if (p[0].ready_t <= time_counter) { //some child executing

            if ((currentP.exec_t-exec_time_counter) > priority_heap[0].exec_t){ //preemptive
                //some process shorter than remaining part
                assert(kill(currentP.pid,9)==0); // kill the origin executing child
                currentP.exec_t -= exec_time_counter; //the remaining time length
                priority_heap[priority_heap_size++] = currentP; //push the remaining to heap
                currentP = priority_heap[0]; //extract new currentP
                #ifndef DEBUG
                syscall(335, &ts_start);
                #endif
                if(currentP.ready_t<time_counter){//this currentP already created
                    fork_pid = fork();
                    if (fork_pid == 0){ // child
                        break;
                    } else if (fork_pid > 0) { // scheduler
                        printf("child recovered at %d. pid = %d\n",time_counter, currentP.pid);
                    }else{
                        printf("fork failed\n");
                    }
                }else{ // create new one , actually currentP.ready_t==time_counter
                    currentP.pid = fork(); // need to execute new preemptive process
                    if (currentP.pid == 0){ // child
                        break;
                    } else if (currentP.pid > 0) { // scheduler
                        printf("child created at %d. pid = %d\n",time_counter, currentP.pid);
                    }else{
                        printf("fork failed\n");
                    }
                }   
                
                swap(&priority_heap[0],&priority_heap[--priority_heap_size]);//swap and the last removed
                ToHeap(priority_heap,priority_heap_size);
                exec_time_counter = 0;
                printf("Preemptive!\n");
            }
            printf("Change control to: ");
            print(currentP);
            sch_p.sched_priority = 4;            
            assert(sched_setscheduler(currentP.pid, SCHED_FIFO, &sch_p) != -1); // let the child run

        }else if (p[0].ready_t > time_counter){ // time i should pass 1 unit if there is no child now
           unit_time(); 
        }
    	
        //#ifdef DEBUG
        if (time_counter % PRINT_INTERVAL == 0){
            printf("<PRINT_INTERVAL>time counter at parent: %d, total_child: %d\n", time_counter,total_child);
        }
        //#endif
    	exec_time_counter++;
    	time_counter++;
    }



    //child-only part
    if (currentP.pid == 0){ 
        child_execution(sch_p, currentP, ts_start, ts_end);
    	//child_execution(sch_p,currentP);
    }

    free(priority_heap);

    //#ifdef DEBUG
    if (total_child == 0)
        printf("finished SJF. total time: %d\n", time_counter); //parent
    //#endif

}

