#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "../util.h"
#include <signal.h>
#include <wait.h>

#include <assert.h>

#define READ_END 0
#define WRITE_END 1
#define BUFFER_SIZE 25
#define TIME_QUANTUM 500
#define PRINT_INTERVAL 250

#define DEBUG 1

int is_terminated = 0;
pid_t exit_pid;
int total_child = 0;

void unit_time() {
    volatile unsigned long i;
    for(i = 0; i < 1000000UL; i++);
}

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

void my_sig_handler(int signum)
{
    if (signum == SIGCHLD)
    {   
        printf("pid: %d terminated\n", getpid());
        exit_pid = getpid();
        is_terminated = 1;
    }
}

int main()
{
    total_child = 3;
    Process p[total_child];
    Process p1 = {"P1", 600, 2000};
    Process p2 = {"P2", 800, 3000};
    Process p3 = {"P3", 1000, 4000};

    p[0] = p1;
    p[1] = p2;
    p[2] = p3;
    
    // set up the sigaction
    struct sigaction sa;

    //sa.sa_handler = my_sig_handler;
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    assert(sigaction(SIGCHLD, &sa, NULL) != -1);
    
    //signal(SIGCHLD, sighandler);

    struct sched_param sch_p;

    pid_t scheduler_pid = getpid();
    printf("scheduler's pid = %d\n", scheduler_pid);
    sch_p.sched_priority = 3;
    
    assert(sched_setscheduler(scheduler_pid, SCHED_FIFO, &sch_p) != -1); // set scheduler's priority to 3

    int current_child_pid;
    int current_child_idx;
    int count_child = 0;
    int ptr_current_process; // for forked child to know which process it need to run


    int RR_start = -1;

    int total_schedule_time = p[0].ready_t;
    
    for (int i = 0; i < 3; ++i)
    {
        total_schedule_time += p[i].exec_t;
    }

    #ifdef DEBUG
    printf("total T: %d\n", total_schedule_time); 
    #endif
  
   
    int time_counter = 0;
    //for (int i = 0; i < total_schedule_time; ++i) 
    while (total_child > 0) //
    {
        for (int j = 0; j < total_child; ++j) // iterate through process to see
        {
            if (time_counter == p[j].ready_t)
            {
                ptr_current_process = j; // For child to remember who they are in the p[] array
                p[j].pid = fork();
                ++count_child;

                if (p[j].pid == 0) // child
                {
                    break;
                } else if (p[j].pid > 0) { // scheduler
                    printf("child %d created at %d. pid = %d\n", j, time_counter, p[j].pid);
                }

                if (count_child == 1) // first child should run
                {
                    current_child_pid = p[j].pid;
                    current_child_idx = j;
                    RR_start = time_counter;
                }
            }
        }
        
        if (count_child != 0 && p[ptr_current_process].pid == 0) { // child
            break;
        }
        
      
        if ( count_child > 1 && (time_counter - RR_start) % 500 == 0 && (time_counter != RR_start)) // should change round
        {
            //current_child_idx = (current_child_idx + 1) % total_child;
            current_child_idx = (current_child_idx + 1) % count_child;
            #ifdef DEBUG
            printf("Current time: %d, child change to: %d. pid = %d\n", time_counter, current_child_idx, p[current_child_idx].pid); 
            #endif
        }
         
        if (count_child > 0) {
            //printf("total_child = %d, current_child_idx = %d\n", total_child, current_child_idx);
            sch_p.sched_priority = 4;
            current_child_pid = p[current_child_idx].pid;
            assert(sched_setscheduler(current_child_pid, SCHED_FIFO, &sch_p) != -1); // let the child run
        }

        unit_time(); // time i has passed
        
        if (is_terminated) {
            printf("pid: %d terminated at time: %d\n", exit_pid, time_counter + 1);
            is_terminated = 0;
            //sch_p.sched_priority = 2; // a running child's prority = 2
            //sched_setscheduler(p[next_run].pid, SCHED_FIFO, &sch_p);
            //++next_run;
            int idx_removed = -1;
            Process tmp;
            
            // remove the exit_child_idx from p[]
            // by moving every one step left
            for (int num = 0; num < total_child; ++num)
            {
                if (p[num].pid == exit_pid)
                {
                    if (num != total_child - 1) // if num == total_child - 1 => then we just --total_child
                    {
                        for (int k = num; k < total_child; ++k) 
                        {
                            // TODO should we keep exit_child's info?
                            p[k] = p[k + 1];
                        }
                        current_child_idx = num; // should set current_child_idx = num, so the next child can continue to run 
                                                 //after some child process finish their execution.
                        
                        #ifdef DEBUG
                        for (int k = 0; k < total_child - 1; ++k) {
                            printf("%s, %d\n", p[k].p_name, p[k].pid);
                        }
                        #endif
                        
                        break;
                    } else if (total_child == 2) { // num == total_child - 1
                        p[0] = p[num];
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
       
        #ifdef DEBUG 
        if (time_counter == 5550)
            printf("time counter:%d, current_child_idx: %d, current_child_pid: %d\n", time_counter, current_child_idx, p[current_child_idx].pid);
        if (time_counter == 5599)
            printf("time counter:%d, current_child_idx: %d, current_child_pid: %d\n", time_counter, current_child_idx, p[current_child_idx].pid);
        #endif
        
        #ifdef DEBUG
        if (time_counter % PRINT_INTERVAL == 0)
        {
            printf("time counter at parent: %d\n", time_counter);
        }
        #endif

        ++time_counter; 
    }


    int total_time = 0;
    if (p[ptr_current_process].pid == 0)
    { 
        int exec_t = p[ptr_current_process].exec_t;
        pid_t cpid = getpid();
        //while (remain_burst[ptr_current_process] > 0)
        //{
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
        
        printf("%s %d\n", p[ptr_current_process].p_name, cpid);
        _exit(0);
        //kill(scheduler_pid, SIGUSR1);

        //}
    }
    
    if (total_child == 0)
        printf("finished RR. total time: %d\n", time_counter);

}
