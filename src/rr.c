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
#define PRINT_INTERVAL 250
#endif

//#ifndef DEBUG
//#define DEBUG 1
//#endif

/* should define these three in main.c */
int is_terminated = 0;
int total_child = 0;
pid_t exit_pid;

//void unit_time() {
//    volatile unsigned long i;
//    for(i = 0; i < 1000000UL; i++);
//}

void sighandler(int signum)
{
    if (signum == SIGCHLD)
    {
        is_terminated = 1;
        exit_pid = wait(NULL);
        
        #ifdef DEBUG
        printf("Child terminated. pid = %d, child left = %d, terminated = %d\n", exit_pid, total_child - 1, is_terminated);
        #endif
    }
}

void rr(Process *p_arr, int N) {

    total_child = N;
    char *tag = "[Project 1]"
    struct timespec ts_start;
    struct timespec ts_start;
    
    /* set up the sigaction */
    struct sigaction sa;

    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    assert(sigaction(SIGCHLD, &sa, NULL) != -1);


    /* set scheduler's priority as 3 */
    struct sched_param sch_p;

    pid_t scheduler_pid = getpid();
    printf("scheduler's pid = %d\n", scheduler_pid);
    sch_p.sched_priority = 3;
   
    /* this sets the scheduler's priority = 3 */
    assert(sched_setscheduler(scheduler_pid, SCHED_FIFO, &sch_p) != -1); // set scheduler's priority to 3

    int current_child_pid;
    int current_child_idx;
    int count_child = 0;
    int ptr_current_process; // for forked child to know 
                             // which process it need to run

    int RR_start = -1;
   
    int time_counter = 0;
    //for (int i = 0; i < total_schedule_time; ++i) 
    while (total_child > 0) //
    {
        for (int j = 0; j < total_child; ++j) // iterate through process to see
        {
            if (time_counter == p_arr[j].ready_t)
            {
                ptr_current_process = j; // For child to remember who they are in the p[] array
                p_arr[j].pid = fork();
                ++count_child;

                if (p_arr[j].pid == 0) // child
                {
                    break;
                } 
                else if (p_arr[j].pid > 0) { // scheduler
                    printf("child %d created at %d. pid = %d\n", j, time_counter, p_arr[j].pid);
                }

                if (count_child == 1) // first child should run
                {
                    current_child_pid = p_arr[j].pid;
                    current_child_idx = j;
                    RR_start = time_counter;
                }
            }
        }
        
        if (count_child != 0 && p_arr[ptr_current_process].pid == 0) { // child
            break;
        }
        
      
        if ( count_child > 1 && (time_counter - RR_start) % 500 == 0 && (time_counter != RR_start)) // should change round
        {
            //current_child_idx = (current_child_idx + 1) % total_child;
            current_child_idx = (current_child_idx + 1) % count_child;
            #ifdef DEBUG
            printf("Current time: %d, child change to: %d. pid = %d\n", time_counter, current_child_idx, p_arr[current_child_idx].pid); 
            #endif
        }
         
        
        if (is_terminated) {
            printf("pid: %d terminated at time: %d\n", exit_pid, time_counter);
            is_terminated = 0;
            //sch_p.sched_priority = 2; // a running child's prority = 2
            //sched_setscheduler(p[next_run].pid, SCHED_FIFO, &sch_p);
            //++next_run;
            int idx_removed = -1;
            Process tmp;
            
            // remove the exit_child_idx from p_arr[]
            // by moving every one step left
            for (int num = 0; num < total_child; ++num)
            {
                if (p_arr[num].pid == exit_pid)
                {
                    if (num != total_child - 1) // if num == total_child - 1 => then we just --total_child
                    {
                        for (int k = num; k < total_child; ++k) 
                        {
                            // TODO should we keep exit_child's info?
                            p_arr[k] = p_arr[k + 1];
                        }
                        current_child_idx = num; // should set current_child_idx = num, so the next child can continue to run 
                                                 //after some child process finish their execution.
                        
                        //#ifdef DEBUG
                        printf("current childs: \n");
                        for (int k = 0; k < total_child - 1; ++k) {
                            printf("%s, %d\n", p_arr[k].p_name, p_arr[k].pid);
                        }
                        //#endif
                        
                        break;
                    } else if (total_child == 2) { // num == total_child - 1
                        //p_arr[0] = p_arr[num];
                        current_child_idx = 0; // should set current_child_idx = 0;
                    }
                }
            }
            --total_child;
            --count_child;
            #ifdef DEBUG
            printf("# of children left: %d\n", total_child);
            #endif
        }
       
        if (count_child > 0) {
            //printf("total_child = %d, current_child_idx = %d\n", total_child, current_child_idx);
            sch_p.sched_priority = 4;
            current_child_pid = p_arr[current_child_idx].pid;
            assert(sched_setscheduler(current_child_pid, SCHED_FIFO, &sch_p) != -1); // let the child run
        }

        if (count_child == 0) // time i should pass 1 unit if there is no child now
            unit_time();
        
        #ifdef DEBUG
        if (time_counter % PRINT_INTERVAL == 0)
        {
            printf("time counter at parent: %d\n", time_counter);
        }
        #endif

        ++time_counter; 
    }

    int total_time = 0;
    if (p_arr[ptr_current_process].pid == 0)
    {
        int exec_t = p_arr[ptr_current_process].exec_t;
        pid_t cpid = getpid();
        
        for(int j = 0; j < exec_t - 1; ++j)
        {
            unit_time();
            sch_p.sched_priority = 2;
            
            #ifdef DEBUG
            if ( (j % PRINT_INTERVAL) == 0)
            {
                printf("child pid: %d, child's time counter: %d\n", cpid, j);
            }
            #endif

            assert(sched_setscheduler(cpid, SCHED_FIFO, &sch_p) != -1); // return control to parent
            ++total_time;
        }
        // last unit of time
        unit_time();     
        ++total_time;   
        printf("child %d stops!, time passed: %d\n", getpid(), total_time);
        
        // should print p_name, pid when it finishs the execution
        printf("%s %d\n", p_arr[ptr_current_process].p_name, cpid);
        _exit(0);

    }
    
    if (total_child == 0)
        printf("finished RR. total time: %d\n", time_counter);

}


