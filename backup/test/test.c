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

void unit_time() {
    volatile unsigned long i;
    for(i = 0; i < 1000000UL; i++);
}

int is_terminated = 0;

void my_sig_handler(int signum)
{
    if (signum == SIGCHLD)
    {   
        printf("pid: %d terminated\n", getpid());
        is_terminated = 1;
    }
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

    struct sigaction sa;

    sa.sa_handler = my_sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    assert(sigaction(SIGCHLD, &sa, NULL) != -1);

    Process ready[3];

    sch_p.sched_priority = 1; // set the priority of scheduler = 1
    pid_t scheduler_pid = getpid();
    sched_setscheduler(scheduler_pid, SCHED_FIFO, &sch_p);

    printf("scheduler pid = %d\n", scheduler_pid);

    pid_t child_pid[3];

    int next_run = 1;
    int count = 0;


    for(int i = 0; i < 24; ++i) {
        if(i == 1) {
            p[0].pid = fork();
            if(p[0].pid == 0) {
                break;

            }
            else if(p[0].pid > 0) {
                close(fd[READ_END]);
                printf("here\n");
                child_pid[0] = p[0].pid;
                sch_p.sched_priority = 2; // a running child's prority = 2
                sched_setscheduler(p[0].pid, SCHED_FIFO, &sch_p);
                count += 1;
            }
            else {
                fprintf(stderr, "FORK FAILED!...");
            }
        }

        if (i == 2) {
            p[1].pid = fork();
            ready[1] = p[1];
            
            if ( p[1].pid == 0 ) {
                break;
            } else { // parent
                child_pid[1] = p[1].pid;
            } 
            
            p[2].pid = fork();
            ready[2] = p[2];

            if ( p[2].pid == 0 ) {
                break;
            }  else { // parent
                child_pid[2] = p[2].pid;
            } 

        }

        if (is_terminated) {
            printf("someone terminated = %d\n", getpid());
            is_terminated = 0;
            sch_p.sched_priority = 2; // a running child's prority = 2
            sched_setscheduler(p[next_run].pid, SCHED_FIFO, &sch_p);
            ++next_run;
        }

        if (count != 0) {
            close(fd[READ_END]); 
            printf("is_terminated = %d, parent: %d\n", is_terminated, i);
            write(fd[WRITE_END], w_msg, strlen(w_msg) + 1);
        }
    }


    if (p[0].pid == 0) {
        close(fd[WRITE_END]);
        for(int j = 0; j < p[0].exec_t; ++j) {
            unit_time();
            printf("child's pid = %d, child: %d\n", getpid(), j);
            read(fd[READ_END], r_msg, BUFFER_SIZE);
            ftruncate(fd[READ_END], 0); // clear contents of fd[READ_END]
        }
        exit(0);
    }
    if (p[1].pid == 0) {
        close(fd[WRITE_END]);
        for(int j = 0; j < p[1].exec_t; ++j) {
            unit_time();
            printf("pid = %d, child: %d\n", getpid(), j);
            read(fd[READ_END], r_msg, BUFFER_SIZE);
        }
        exit(0);
    }
    if (p[2].pid == 0) {
        close(fd[WRITE_END]);
        for(int j = 0; j < p[2].exec_t; ++j) {
            unit_time();
            printf("pid = %d, child: %d\n", getpid(), j);
            read(fd[READ_END], r_msg, BUFFER_SIZE);
        }
        exit(0);
    }
}
