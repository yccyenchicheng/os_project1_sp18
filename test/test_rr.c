#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "../util.h"
#include <signal.h>

#include <assert.h>

#define READ_END 0
#define WRITE_END 1
#define BUFFER_SIZE 25
#define TIME_QUANTUM 500

int is_terminated = 0;
pid_t exit_pid;

void unit_time() {
    volatile unsigned long i;
    for(i = 0; i < 1000000UL; i++);
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
    Process p[3];
    Process p1 = {"P1", 600, 4000};
    Process p2 = {"P2", 800, 5000};
    Process p3 = {"P3", 1000, 6000};

    p[0] = p1;
    p[1] = p2;
    p[2] = p3;
    
    // set up the sigaction
    struct sigaction sa;

    sa.sa_handler = my_sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    assert(sigaction(SIGCHLD, &sa, NULL) != -1);
     

    int remain_burst[3];
    
    for (int i = 0; i < 3; ++i)
        remain_burst[i] = p[i].exec_t;

    struct sched_param sch_p;

    int count = 0;
    int next_run = 0;

    pid_t scheduler_pid = getpid();
    printf("scheduler's pid = %d\n", scheduler_pid);
    sch_p.sched_priority = 3;
    
    assert(sched_setscheduler(scheduler_pid, SCHED_RR, &sch_p) != -1); // set scheduler's priority to 3

    int current_child_pid;
    int current_child_idx;
    int count_child = 0;
    int ptr_current_process; // for forked child to know which process it need to run


    int RR_start;
    int total_child = 3;

    int total_schedule_time = p[0].ready_t;
    
    for (int i = 0; i < 3; ++i)
    {
        total_schedule_time += p[i].exec_t;
    }

    #ifdef DEBUG
    printf("total T: %d\n", total_schedule_time); 
    #endif
    
    for (int i = 0; i < total_schedule_time; ++i) 
    { // 600 + 4000 + 5000 + 6000
        for (int j = 0; j < 3; ++j) // iterate through process to see
        {
            if (i == p[j].ready_t)
            {
                ptr_current_process = j; // For child to remember who they are in the p[] array
                p[j].pid = fork();
                ++count_child;

                if (p[j].pid == 0) // child
                {
                    break;
                } 
                else if (p[j].pid > 0) // scheduler
                {
                    printf("child %d created at %d. pid = %d\n", j, i, p[j].pid);

                }

                if (count_child == 1) // first child should run
                {
                    current_child_pid = p[j].pid;
                    current_child_idx = j;
                    RR_start = i;
                }
            }
        }
        
        if (is_terminated) {
            printf("someone terminated\n");
            is_terminated = 0;
            //sch_p.sched_priority = 2; // a running child's prority = 2
            //sched_setscheduler(p[next_run].pid, SCHED_FIFO, &sch_p);
            //++next_run;
            --total_child;
            current_child_idx = (current_child_idx + 1) % total_child;
        }


        if (count_child != 0 && p[ptr_current_process].pid == 0) // child
            break;
      
        if ( (i - RR_start) % 500 == 0 && (i != RR_start) ) // should change round
        {
            current_child_idx = (current_child_idx + 1) % total_child;
            printf("child change to: %d. %d\n", current_child_idx, p[current_child_idx].pid); 
        }
         
        if (count_child != 0) {
            sch_p.sched_priority = 4;
            current_child_pid = p[current_child_idx].pid;
            assert(sched_setscheduler(current_child_pid, SCHED_FIFO, &sch_p) != -1); // let the child run
        }

        unit_time(); // time i has passed
        
        /* 
        if (i == 0) {
            ptr_current_process = 0;
            p[0].pid = fork();
            ++count_child;

            if(p[0].pid == 0) {
                break;
            }
            else if(p[0].pid > 0) {
                printf("Create child at time %d\n, p[0].pid: %d\n", i, p[0].pid);
                sch_p.sched_priority = 4; // let it run
                current_child_pid = p[0].pid;
                current_child_idx = 0;
                
                assert(sched_setscheduler(current_child_pid, SCHED_FIFO, &sch_p) != -1);
            }
            else {
                fprintf(stderr, "FORK FAILED!...");
            }
        }
        
        if (i == 2) {
            ptr_current_process = 1;
            p[1].pid = fork();
            ++count_child;

            if(p[1].pid == 0) {
                break;
            } else if (p[1].pid > 0) {
                printf("Create child at time %d\n, p[2].pid: %d\n", i, p[1].pid);
            } 
            
            ptr_current_process = 2;
            p[2].pid = fork();
            ++count_child;

            if (p[2].pid == 0) {
                break;
            } else if (p[2].pid > 0) {
                printf("Create child at time %d\n, p[2].pid: %d\n", i, p[2].pid);
            }
        }
        
        if (i == 11) {
            ++current_child_idx; // next child should run
        }

        if (i == 16) {
            ++current_child_idx; // next child should run
        }*/
        
        if (i % 250 == 0)        
            printf("time counter at parent: %d\n", i);
        
    }


    //close(fd[WRITE_END]);
    
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
            
            if ( (j + 1) % 250 == 0) 
                printf("child pid: %d, time has passed: %d\n", cpid, j + 1);
            
            assert(sched_setscheduler(cpid, SCHED_FIFO, &sch_p) != -1); // return control to parent
            ++total_time;
        }
        // last unit of time
        unit_time();     
        ++total_time;   
        //printf("child %d stops!, time passed: %d\n", getpid(), total_time);
        _exit(0);
        //}
    }
}
