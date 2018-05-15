#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <wait.h>
#include <assert.h>

/* for system call */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <time.h>
#include <sys/syscall.h>


#include "util.h"

#ifndef TIME_QUANTUM
#define TIME_QUANTUM 500
#endif 

#ifndef PRINT_INTERVAL 
#define PRINT_INTERVAL 100
#endif 

int is_terminated = 0;
int total_child = 0;
pid_t exit_pid;

void sighandler(int signum)
{
    if (signum == SIGCHLD)
    {
        is_terminated = 1;
        exit_pid = wait(NULL);
        
        #ifdef DEBUG
        printf("Child terminated. pid = %d, child left = %d, terminated = %d\n\n", exit_pid, total_child - 1, is_terminated);
        #endif
    }
}

void rr(Process *p_arr, int N) {
    
    total_child = N;
    char *tag = "[Project1]";
    
    struct timespec ts_start;
    struct timespec ts_end;
    
    /* set up the sigaction */
    struct sigaction sa;

    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    assert(sigaction(SIGCHLD, &sa, NULL) != -1);


    /* set scheduler's priority as 3 */
    struct sched_param sch_p;

    pid_t scheduler_pid = getpid();
    #ifdef DEBUG
    printf("scheduler's pid = %d\n", scheduler_pid);
    #endif
    sch_p.sched_priority = 3;
   
    /* this sets the scheduler's priority = 3 */
    assert(sched_setscheduler(scheduler_pid, SCHED_FIFO, &sch_p) != -1); // set scheduler's priority to 3

    int current_child_pid;
    int current_child_idx;
    int count_child = 0;
    int ptr_current_process; // for forked child to know 
                             // which process it need to run

    int RR_start = -1;
 
    struct queue *ready_q = (struct queue*) malloc(sizeof(struct queue));
    
    ready_q->stack1 = NULL;
    ready_q->stack2 = NULL;
    //ready_q->front = NULL;
    //ready_q->end = NULL;

    //Process ready_q[N];
   
    int time_counter = 0;
    while (total_child > 0) //
    {
        #ifdef DEBUG
        if (time_counter % PRINT_INTERVAL == 0)
        {
            printf("Parent at start of time: %d\n", time_counter);
        }
        #endif

        for (int i = 0; i < total_child; ++i) // iterate through process to see
        {
            if (time_counter == p_arr[i].ready_t)
            {
                ptr_current_process = i; // For child to remember who they are in the p[] array

                syscall(335, &ts_start); // for printk
                p_arr[i].pid = fork();
                enQueue(ready_q, p_arr[i]); // enQueue the forked child at the end of the queue

                ++count_child;

                if (p_arr[i].pid == 0) // child
                {
                    break;
                } 
                else if (p_arr[i].pid > 0) { // scheduler
                    #ifdef DEBUG
                    printf("checking ready_q. front is: ");
                    print(ready_q->front);
                    printf("checking ready_q.   end is: ");
                    print(ready_q->end);
                    #endif
                    
                    #ifdef DEBUG
                    printf("%s created at %d. pid = %d. %s is running.\n", p_arr[i].p_name,time_counter, p_arr[i].pid, ready_q->front.p_name);
                    #endif
                }

                if (count_child == 1) // first child should run
                {
                    //current_child_pid = p_arr[i].pid;
                    current_child_pid = ready_q->front.pid; 
                    current_child_idx = i;
                    RR_start = time_counter;
                }
            }
        }
        
        //if (count_child != 0 && p_arr[ptr_current_process].pid == 0 && getpid() != scheduler_pid) { // child
        if (count_child != 0 && getpid() != scheduler_pid) {
            break;
        }
        
        /* Change round.
         * Should also check if someone just terminated its execution */
        if (count_child > 1 && (time_counter - RR_start) % TIME_QUANTUM == 0 && (time_counter != RR_start))
        {
            //if ( !(is_terminated && p_arr[current_child_idx].pid == exit_pid) ) {
            if ( !(is_terminated && ready_q->front.pid == exit_pid) ) {
                //current_child_idx = (current_child_idx + 1) % count_child;
                Process tmp = deQueue(ready_q);
                enQueue(ready_q, tmp);
                #ifdef DEBUG
                printf("Time: %d, change from %s, %d to %s, %d\n", time_counter, tmp.p_name, tmp.pid, ready_q->front.p_name, ready_q->front.pid);
                #endif
            }
        }
         
        
        if (is_terminated) {
            #ifdef DEBUG
            printf("pid: %d terminated at time: %d\n", exit_pid, time_counter);
            #endif
            is_terminated = 0;
            int idx_removed = -1;
            Process terminated_child = deQueue(ready_q);
            
            --total_child;
            --count_child;
            #ifdef DEBUG
            printf("%s, pid: %d stops. Next: %s, pid: %d\n", terminated_child.p_name, terminated_child.pid, ready_q->front.p_name, ready_q->front.pid);
            #endif
        }
       
        if (count_child > 0) {
            //printf("total_child = %d, current_child_idx = %d\n", total_child, current_child_idx);
            sch_p.sched_priority = 4;
            //current_child_pid = p_arr[current_child_idx].pid;
            current_child_pid = ready_q->front.pid;
            #ifdef DEBUG
            if (time_counter % PRINT_INTERVAL == 0)
                printf("current_child_idx = %d, current_child_pid = %d\n", current_child_idx, current_child_pid);
            
            #endif
            
            #ifdef DEBUG
            printf("### change to child ###\n");
            #endif
            //printf("cur pid = %d\n", current_child_pid);
            assert(sched_setscheduler(current_child_pid, SCHED_FIFO, &sch_p) != -1); // let the child run
        }

        if (count_child == 0) // time i should pass 1 unit if there is no child now
            unit_time();
        

        ++time_counter; 
    }

    int total_time = 0;
    pid_t cpid;
    if ( (cpid = getpid()) != scheduler_pid )
    {
        int exec_t = p_arr[ptr_current_process].exec_t;
        
        for(int j = 0; j < exec_t - 1; ++j)
        {
            #ifdef DEBUG
            if ( (j % PRINT_INTERVAL) == 0)
            {
                printf("Child  at start of time: %d, pid: %d\n", j, cpid);
            }
            #endif
            unit_time();
            sch_p.sched_priority = 2;
            
            
            #ifdef DEBUG
            printf("### change to parent ###\n");
            #endif
            assert(sched_setscheduler(cpid, SCHED_FIFO, &sch_p) != -1); // return control to parent
            ++total_time;
        }
        /* last unit of time */
        #ifdef DEBUG
        printf("Child  at start of time: %d, pid: %d\n", total_time, cpid);
        #endif
        unit_time();     
        syscall(335, &ts_end); // for printk
        ++total_time;
        #ifdef DEBUG
        printf("child %d stops!, time passed: %d\n", getpid(), total_time);
        printf("%s, pid: %d is about to exit!\n", p_arr[ptr_current_process].p_name, getpid());
        #endif

        /* should print p_name, pid when it finishs the execution */
        printf("%s %d\n", p_arr[ptr_current_process].p_name, cpid);
        /* for dmesg */
        syscall(334, tag, cpid, &ts_start, &ts_end); // for dmesg
        
        _exit(0);

    }
    
    #ifdef DEBUG
    if (total_child == 0)
        printf("finished RR. total time: %d\n", time_counter);
    #endif
}


