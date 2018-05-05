#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "../util.h"

#include <assert.h>

#define READ_END 0
#define WRITE_END 1
#define BUFFER_SIZE 25

void unit_time() {
    volatile unsigned long i;
    for(i = 0; i < 1000000UL; i++);
}




int main()
{
    Process p[3];
    Process p1 = {"P1", 1, 10};
    Process p2 = {"P2", 2, 5};
    Process p3 = {"P3", 2, 7};

    p[0] = p1;
    p[1] = p2;
    p[2] = p3;

    struct sched_param sch_p;
    int fd[2];   pipe(fd);
    char w_msg[BUFFER_SIZE] = "hello, world";
    char r_msg[BUFFER_SIZE];

    int count = 0;
    int next_run = 0;

    pid_t scheduler_pid = getpid();
    printf("scheduler's pid = %d\n", scheduler_pid);
    sch_p.sched_priority = 3;
    
    assert(sched_setscheduler(scheduler_pid, SCHED_FIFO, &sch_p) != -1);

    int current_child_pid;
    int current_child_idx;
    int count_child = 0;
    int ptr_current_process; // for forked child to know which process it need to run

    int i; 
    for(i = 0; i < 24; ++i) {
        if (i == 1) {
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
        }
        
        printf("time counter at parent: %d\n", i);
        
        
        if (count_child != 0) {
            sch_p.sched_priority = 4;
            current_child_pid = p[current_child_idx].pid;
            assert(sched_setscheduler(current_child_pid, SCHED_FIFO, &sch_p) != -1);
        }
    }


    //close(fd[WRITE_END]);
    
    int total_time = 0;
    if (p[ptr_current_process].pid == 0) {
        int exec_t = p[ptr_current_process].exec_t;
        pid_t cpid = getpid();
        for(int j = 0; j < exec_t - 1; ++j) {
            unit_time();
            sch_p.sched_priority = 2;
            printf("child pid: %d, time has passed: %d\n", cpid, j + 1);
            assert(sched_setscheduler(cpid, SCHED_FIFO, &sch_p) != -1); // return control to parent
            ++total_time;
        }
        // last unit of time
        unit_time();     
        ++total_time;   
    }
    printf("child %d stops!, time passed: %d\n", getpid(), total_time);
}
