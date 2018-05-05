#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "../util.h"
#include <wait.h>

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

    pid_t sched_pid = getpid();

    printf("parent id = %d\n", sched_pid);

    sch_p.sched_priority = 99;
    sched_setscheduler(sched_pid, SCHED_FIFO, &sch_p);

    //for (int i = 0; i < 3; ++i)
    //   printf("%s, %d, %d\n", p[i].p_name, p[i].ready_t, p[i].exec_t);
    int i;
    
    for (i = 0; i < 3; ++i) {
        p[i].pid = fork();
        if (p[i].pid == 0) { // just want 3 child
            break;
        }
    }

    if (getpid() == sched_pid) {
        for (i = 0; i < 3; ++i) {
            sch_p.sched_priority = i + 1;
            printf("i = %d, child_pid = %d\n", i, p[i].pid);
            sched_setscheduler(p[i].pid, SCHED_FIFO, &sch_p);
        }
        wait(NULL);
    } else {
        printf("child!\n");
        for(int j = 0; j < 5; ++j) {
            unit_time();
            printf("%d, child: %d\n", getpid(), j);
        }
    }

    return 0;
}
