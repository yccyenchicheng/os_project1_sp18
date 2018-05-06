#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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
   //initialize processes
   Process p[3];
   Process p1 = {"P1", 0, 6};
   Process p2 = {"P2", 0, 3};
   Process p3 = {"P3", 7, 3};

   p[0] = p1;
   p[1] = p2;
   p[2] = p3;

   Process *rbegin, *rend, *wbegin, *wend;
   rbegin = &p[0];
   rend   = &p[0];
   wbegin = &p[0];
   wend   = (&p[2]+1);

   struct sched_param sch_p;
       
   sch_p.sched_priority = 3;   
    
   assert(sched_setscheduler(getpid(), SCHED_FIFO, &sch_p) != -1);

   sch_p.sched_priority = 4;   
   
   int t, r, status;   t = 0;   r = 0;
   pid_t pid;   pid = getpid();
   while((rbegin != rend) || (wbegin != wend)) {
      printf("t = %d: ", t);   fflush(stdout);
      if(wbegin != wend) {
         while(wbegin->ready_t == t) {
            printf("%s ready... ", wbegin->p_name);   fflush(stdout); 
            pid = fork();   wbegin->pid = pid;
            if(pid < 0) {
               perror("fork() failed...\n");
            }
            else if(pid == 0) {
               pid_t cpid = getpid();
               sch_p.sched_priority = 2;
               for(int i = 0; i < wbegin->exec_t-1; ++i) {
                  unit_time();
                  printf("\n   %s run... \n", wbegin->p_name);   fflush(stdout);
                  assert(sched_setscheduler(cpid, SCHED_FIFO, &sch_p) != -1);
               }       
               unit_time();  
               printf("\n   %s run... \n", wbegin->p_name);   fflush(stdout);
               exit(0);
            }
            else {
               ++wbegin;
               ++rend;
            } 
         }
      } 

      // scheduler schedules processes from rbegin to rend

      if((r == 0) && (rbegin != rend)) {
         assert(sched_setscheduler(rbegin->pid, SCHED_FIFO, &sch_p) != -1);
         ++r;
      } 
      else if(r == 1) {
         if(waitpid(rbegin->pid, &status, WNOHANG) != 0) {
            ++rbegin; 
            --r; 
            if(rbegin != rend) {
               assert(sched_setscheduler(rbegin->pid, SCHED_FIFO, &sch_p) != -1);
               ++r;
            }
            else {
               unit_time();
               printf("\n   S idle...\n");   fflush(stdout);
            }
         }
         else {
            assert(sched_setscheduler(rbegin->pid, SCHED_FIFO, &sch_p) != -1);
         }
      }      
      else {
         unit_time();
         printf("\n   S idle...\n");   fflush(stdout);
      }       
      printf("\n");
      ++t;
   } 
}
