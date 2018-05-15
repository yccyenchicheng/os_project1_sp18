#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "util.h"
#include "scheduler.h"

#ifndef BUFF_SIZE
#define BUFF_SIZE 32
#endif

int main(int argc, char *argv[]) {
    
    /*
    FILE* fp = argv[1];
    if (! fp.isopen() ) {
        fprintf(stderr, "File open error");
    }*/
    
    char schedule_policy[10];
    int N;

    int ready_t;
    int exec_t;


    fscanf(stdin, "%s\n", schedule_policy);
    fscanf(stdin, "%d\n", &N);

    Process p_arr[N];


    int i;
    for (i = 0; i < N; ++i) {
        char p_name_buff[BUFF_SIZE];
         
        fscanf(stdin, "%s %d %d\n", p_name_buff, &ready_t,
                                    &exec_t);
        Process tmp_p;
        memcpy(tmp_p.p_name, p_name_buff, BUFF_SIZE);
        tmp_p.p_name[BUFF_SIZE - 1] = 0;
        tmp_p.ready_t = ready_t;
        tmp_p.exec_t  = exec_t;
        //strcpy(p_arr[i].p_name, p_name_buff);
        
        p_arr[i] = tmp_p;
    }
 
   QsortReady(p_arr, N);
 
   if(strcmp(schedule_policy, "FIFO") == 0) {
      fifo(p_arr, N);      
   }
   else if(strcmp(schedule_policy, "RR") == 0) {
      rr(p_arr, N);
   }
   else if(strcmp(schedule_policy, "SJF") == 0) {
      sjf(p_arr, N);
   }
   else {
      psjf(p_arr, N);
   } 


   
 
    return 0;
}

