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

static int is_terminated = 0;
static pid_t exit_pid;
static int total_child = 0;


void sjf_sighandler(int signum){
    if (signum == SIGCHLD){
        is_terminated = 1;
        exit_pid = wait(NULL);
    }
}

void sjf(Process* p,int N){
    
    char *tag = "[Project1]";
    struct timespec ts_start;
    struct timespec ts_end;

    struct sigaction sa;
    sa.sa_handler = sjf_sighandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    assert(sigaction(SIGCHLD, &sa, NULL) != -1);
    // this part is to handle death of child process 

    struct sched_param sch_p;
    pid_t scheduler_pid = getpid();
    
    
    sch_p.sched_priority = 3;
    assert(sched_setscheduler(scheduler_pid, SCHED_FIFO, &sch_p) != -1); 


    QsortReady(p,N);//sort ready_t from small to large
    

    total_child = N;
    Process currentP;//Now executing process 
    currentP.pid = -1;
    currentP.ready_t = -1;
    currentP.exec_t = -1; // smaller then zero when before the "currentP = first process" so that won't fork child
    Process* priority_heap = (Process*)malloc(N* sizeof(Process));
    int priority_heap_size = 0;
    int time_counter = 0;
    
    int ready_index = 0; //check if p[ready_index] ready
    while (total_child > 0){ // main parent loop
    	//check counter if some other process ready,add to priority_heap, if ready_index=0 fork and run immediately
    	//while check whether p[ready_index] ready
    	while(ready_index<N && p[ready_index].ready_t==time_counter){
                   
            
            priority_heap[priority_heap_size++] = p[ready_index];	//add this process to priority_heap
            ToHeap(priority_heap,priority_heap_size);//clean but maybe slow QQ



            ready_index++;
    	}
    	//reset exec_time_counter=0
        if (p[0].ready_t == time_counter){
            is_terminated = 1;
            total_child++; //before the first process start, no need to --total_child in line 143, so need to print N there in first time
        }


    	if(is_terminated){//if execution time end, print counter
            
            is_terminated = 0;
            
            if(priority_heap_size>0){//if no process running or ready, idle

                currentP = priority_heap[0];
                syscall(335, &ts_start);
                currentP.pid = fork();
                if (currentP.pid == 0){ // child
                    break;
                } else if (currentP.pid > 0) { // scheduler
                    //printf("child created at %d. pid = %d\n",time_counter, currentP.pid);
                }
                swap(&priority_heap[0],&priority_heap[--priority_heap_size]);//the last removed
                ToHeap(priority_heap,priority_heap_size);//clean but maybe slow QQ
                
                sch_p.sched_priority = 4;
                assert(sched_setscheduler(currentP.pid, SCHED_FIFO, &sch_p) != -1); // let the child run
            }
    		
    	}else if (p[0].ready_t <= time_counter) { //some child executing
            sch_p.sched_priority = 4;            
            assert(sched_setscheduler(currentP.pid, SCHED_FIFO, &sch_p) != -1); // let the child run
        }else if (p[0].ready_t > time_counter){ // time i should pass 1 unit if there is no child now
           unit_time(); 
        }
    	
        
    	time_counter++;
    }



    //child-only part
    if (currentP.pid == 0){ 
        child_execution(sch_p, currentP, ts_start, ts_end);
    	//child_execution(sch_p,currentP);
    }

    free(priority_heap);


}

