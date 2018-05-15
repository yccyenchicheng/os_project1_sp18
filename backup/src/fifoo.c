#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#include "scheduler.h"
#include "util.h"

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); \
} while(0)

void fifo(Process* p_arr, int N) {
   Process *rbegin, *rend, *wbegin, *wend;
   rbegin = rend = wbegin = &p_arr[0];
   wend   = (&p_arr[N]);

   struct sched_param sch;
   sch.sched_priority = 1;

   int parentCPU, childCPU;
   cpu_set_t cpu;
   parentCPU = 0;
   childCPU  = 1;
   CPU_ZERO(&cpu); 
   CPU_SET(parentCPU, &cpu); 
   if(sched_setaffinity(getpid(), sizeof(cpu), &cpu) == -1) {
      errExit("sched_setaffinity");   
   } 

   int time, run, status;   
   time = run = 0;
   while((rbegin != rend) || (wbegin != wend)) {
      if(wbegin != wend) {
         while(wbegin->ready_t == time) {
            wbegin->pid = fork();

            switch(wbegin->pid) {
            case -1:
               errExit("fork");
            case 0:
               CPU_ZERO(&cpu);
               CPU_SET(childCPU, &cpu);
               if(sched_setaffinity(getpid(), sizeof(cpu), &cpu) == -1) {
                  errExit("sched_setaffinity");   
               } 

               for(int i = 0; i < wbegin->exec_t; ++i) {
                  unit_time();
               }
             
               printf("%s %d\n", rbegin->p_name, rbegin->pid);   fflush(stdout);

               exit(EXIT_SUCCESS);
            default:   
               /* Parent */
               ++wbegin;
               ++rend;
            }
         }
      }

      switch(run) {
      case 0:
         if(rbegin != rend) {
            switch(sched_setscheduler(rbegin->pid, SCHED_FIFO, &sch)) {
            case -1:
               errExit("sched_sescheduler");
            default:
               ++run;
            }
         }
      default:
         if(waitpid(rbegin->pid, &status, WNOHANG) == -1) {
            errExit("waitpid");
         }
         else if(waitpid(rbegin->pid, &status, WNOHANG) == rbegin->pid) {

            ++rbegin;
            --run;
            if(rbegin != rend) {
               switch(sched_setscheduler(rbegin->pid, SCHED_FIFO, &sch)) {
               case -1:
                  errExit("sched_sescheduler");
               default:
                  ++run;
               }
            }
         } 
         else {

         }
      }
   
      unit_time(); 
   }

}
