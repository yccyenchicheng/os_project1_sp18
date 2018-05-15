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

int is_terminated = 0;
int total_child = 0;
pid_t exit_pid;

void sighandler(int signum)
{
    if (signum == SIGCHLD)
    {
        is_terminated = 1;
        exit_pid = wait(NULL);
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
   
    int time_counter = 0;
    while (total_child > 0) //
    {

        for (int i = 0; i < N; ++i) // iterate through process to see
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
                    ;
                }

                if (count_child == 1) // first child should run
                {
                    current_child_pid = ready_q->front.pid; 
                    current_child_idx = i;
                    RR_start = time_counter;
                }
            }
        }
        
        if (count_child != 0 && getpid() != scheduler_pid) {
            break;
        }
        
        /* Change round.
         * Should also check if someone just terminated its execution */
        if (count_child > 1 && (time_counter - RR_start) % TIME_QUANTUM == 0 && (time_counter != RR_start))
        {
            if ( !(is_terminated && ready_q->front.pid == exit_pid) ) {
                Process tmp = deQueue(ready_q);
                enQueue(ready_q, tmp);
            }
        }
         
        
        if (is_terminated) {
            is_terminated = 0;
            int idx_removed = -1;
            Process terminated_child = deQueue(ready_q);
            
            --total_child;
            --count_child;
        }
       
        if (count_child > 0) {
            sch_p.sched_priority = 4;
            current_child_pid = ready_q->front.pid;
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
            unit_time();
            sch_p.sched_priority = 2;
            
            assert(sched_setscheduler(cpid, SCHED_FIFO, &sch_p) != -1); // return control to parent
            ++total_time;
        }
        /* last unit of time */
        unit_time();     
        syscall(335, &ts_end); // for printk
        ++total_time;

        /* should print p_name, pid when it finishs the execution */
        printf("%s %d\n", p_arr[ptr_current_process].p_name, cpid);
        /* for dmesg */
        syscall(334, tag, cpid, &ts_start, &ts_end); // for dmesg
        
        _exit(0);

    }
    
}


