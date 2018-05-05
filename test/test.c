#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "../util.h"


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

   //for (int i = 0; i < 3; ++i)
   //   printf("%s, %d, %d\n", p[i].p_name, p[i].ready_t, p[i].exec_t);
   
   for(int i = 0; i < 24; ++i) {
      if(i == 1) {
         p[0].pid = fork();
         if(p[0].pid == 0) {
            close(fd[WRITE_END]);
            for(int j = 0; j < p[0].exec_t; ++j) {
               unit_time();
               printf("child: %d\n", j);
               read(fd[READ_END], r_msg, BUFFER_SIZE);
            }
            break;
      
         }
         else if(p[0].pid > 0) {
            close(fd[READ_END]);
            sch_p.sched_priority = 1;
            sched_setscheduler(p[0].pid, SCHED_FIFO, &sch_p);
         }
         else {
            fprintf(stderr, "FORK FAILED!...");
         }
      }

      close(fd[READ_END]); 
      printf("partent: %d\n", i);
      write(fd[WRITE_END], w_msg, strlen(w_msg) + 1);
       
   } 
}
