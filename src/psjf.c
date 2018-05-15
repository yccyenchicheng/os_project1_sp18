#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "scheduler.h"
#include "util.h"

#include <assert.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <time.h>
#include <sys/syscall.h>

#define READ_END 0
#define WRITE_END 1
#define BUFFER_SIZE 25

void psjf(Process* p_arr, int N) {
   char *tag = "[Project1]";
    
   struct timespec ts_start;
   struct timespec ts_end;

   Process *rbegin, *rend, *wbegin, *wend;
   Process current;
   rbegin = rend = wbegin = &p_arr[0];
   wend   = (&p_arr[N]);

   struct sched_param sch_p; 
   sch_p.sched_priority = 3;    
   assert(sched_setscheduler(getpid(), SCHED_FIFO, &sch_p) != -1);
   sch_p.sched_priority = 4;   
   
   int t, r, total, status;   t = 0;   r = 0;   total = N;

   // initialize heap
   Process h[N]; 
   myHeap heap; 
   heap.current_size = 0;
   heap.max_size = N;
   heap.container = h;

   pid_t pid;   pid = getpid();
   while(total > 0) {
         while((wbegin != wend) && (wbegin->ready_t == t)) {
            syscall(335, &ts_start);

            pid = fork();   wbegin->pid = pid;
            if(pid < 0) {
               perror("fork() failed...\n");
            }
            else if(pid == 0) {
               pid_t cpid = getpid();
               sch_p.sched_priority = 2;
               for(int i = 0; i < wbegin->exec_t-1; ++i) {
                  unit_time();
                  assert(sched_setscheduler(cpid, SCHED_FIFO, &sch_p) != -1);
               }       
               unit_time();  
              
               syscall(335, &ts_end);
               syscall(334, tag, getpid(), &ts_start, &ts_end);
 
               printf("%s %d\n", wbegin->p_name, getpid());   fflush(stdout);
               exit(0);
            }
            else {
               assert(myPush(&heap, wbegin));

               ++wbegin;
               ++rend;
            } 
         } 

      if((r == 0) && (!myEmpty(&heap))) {      
         assert(myFront(&heap, &current));

         assert(myPop(&heap)); 
  
         assert(sched_setscheduler(current.pid, SCHED_FIFO, &sch_p) != -1);

         ++r;
      } 
      else if(r == 1) {
         if(waitpid(current.pid, &status, WNOHANG) != 0) {
            ++rbegin; 
            --r; 
            --total; 
            
            if(!myEmpty(&heap)) {           
               assert(myFront(&heap, &current));

               assert(myPop(&heap)); 
 
               assert(sched_setscheduler(current.pid, SCHED_FIFO, &sch_p) != -1);

               ++r;
            }
            else {
               unit_time();
            }
         }
         else {
            if(!myEmpty(&heap)) {
               Process front;
               assert(myFront(&heap, &front));
               if(current.exec_t > front.exec_t) {
                  myPush(&heap, &current);
                  
                  myFront(&heap, &current);

                  myPop(&heap);
               }               
            }
            
            --(current.exec_t);
            
            assert(sched_setscheduler(current.pid, SCHED_FIFO, &sch_p) != -1);
         }
      }      
      else {
         unit_time();
      }       
      ++t;
   } 
}


