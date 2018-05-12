#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "util.h"
#include "scheduler.h"

#define DEBUG 1

#ifndef BUFF_SIZE
#define BUFF_SIZE 32
#endif

void print(Process p) {
    printf("%s %d %d\n", p.p_name, p.ready_t, p.exec_t);
}

void unit_time() {
    volatile unsigned long i;
    for (i = 0; i < 1000000UL; i++);
}

int main(int argc, char *argv[]) {
    
    /*
    FILE* fp = argv[1];
    if (! fp.isopen() ) {
        fprintf(stderr, "File open error");
    }*/
    
    char schedule_policy[10];
    int N;


    //char p_name_buff[BUFF_SIZE];
    int ready_t;
    int exec_t;


    fscanf(stdin, "%s\n", schedule_policy);
    fscanf(stdin, "%d\n", &N);

    printf("Buff_size = %d\n", BUFF_SIZE);
    Process p_arr[N];

    #ifdef DEBUG
    fprintf(stdout, "%s\n", schedule_policy);
    fprintf(stdout, "%d\n", N);
    #endif

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
        #ifdef DEBUG
        printf("i = %d, p_name = %s, p_name's address: %p\n", 
                i, p_arr[i].p_name, (p_arr[i].p_name));
        #endif
    }
 
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


   
    #ifdef DEBUG
    printf("checking value\n");
    for (i = 0; i < N; ++i) {
        printf("%d ", i);
        print(p_arr[i]);
    }
    #endif   

    /* set process priority */
    struct sched_param param;
    
    /* store process ids of children */
    pid_t pid[N];
 
    return 0;
}

