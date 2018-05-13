#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <wait.h>
#include <assert.h>
#include "util.h"
#define BUFF_SIZE 32
#define PRINT_INTERVAL 250
#define DEBUG 1
/* Program to implement a queue using two stacks */
//extern struct Process;
void print(Process p) {
    printf("%s %d %d\n", p.p_name, p.ready_t, p.exec_t);
}

void unit_time() {
    volatile unsigned long i;
    for(i = 0; i < 1000000UL; i++);
}

int str_equal(char* c1,char* c2){
    int i = 0;
    while(c1[i]==c2[i]){

        if(c1[i]=='\0'){
            #ifdef DEBUG
            printf("shorter strlen = %d\n",i);
            #endif
            return 1;
        }
        i++;
    }
    return 0;

}



void child_execution(struct sched_param sch_p,Process currentP){
    int total_time = 0;
    int exec_t = currentP.exec_t;//p[ptr_current_process].exec_t;
    pid_t cpid = getpid();
    for(int j = 0; j < exec_t - 1; ++j){
        unit_time();
        sch_p.sched_priority = 2;
        #ifdef DEBUG
        if ( (j % PRINT_INTERVAL) == 0){
            printf("child pid: %d, child's time counter: %d\n", cpid, j);
        }
        #endif
        assert(sched_setscheduler(cpid, SCHED_FIFO, &sch_p) != -1); // return control to parent
        ++total_time;
    }
    // last unit of time
    unit_time();     
    ++total_time;   
    printf("child %s with pid:%d stops!, time passed: %d\n",currentP.p_name,cpid, total_time);
    _exit(0);
}

void ToHeap(Process* p,int N){
    if(N==1){
      
        printf("Build heap:\n");
        print(p[0]);
        return;
    }
    #ifdef DEBUG
    printf("Before heapify:\n");
    for(int i=0;i<N;i++){
        print(p[i]);
    }
    #endif
    for(int i=(N>>1)-1;i>=0;i--){
        MaxHeapify(p,N,i);
    }
    #ifdef DEBUG
    printf("After heapify:\n");
    for(int i=0;i<N;i++){
        print(p[i]);
    }
    #endif
}
void MaxHeapify(Process* p,int N,int index){
    int largest = 0;
    int left = (index<<1) + 1, right = (index<<1) + 2;
    //printf("index:%d,left:%d,right:%d\n",index,left,right);
    if(left<N && largerP(p[left],p[index])){
        largest = left;
    }else{
        largest = index;
    }
    if(right<N && largerP(p[right],p[largest])){
        largest = right;
    }
    if (largest!=index){
        swap(&p[largest],&p[index]);
        MaxHeapify(p,N,largest);
    }

}

int largerP(Process p1,Process p2){ //return true if priority p1 > p2 (SJF)
    // printf("Comparing...\n");
    // print(p1);
    // print(p2);
    if(p1.exec_t<p2.exec_t||(p1.exec_t==p2.exec_t&&p1.ready_t<p2.ready_t)){
        return 1;
    }else{
        return 0;
    }
}
int earlierP(Process p1,Process p2){ //return true if ready_t p1 < p2 
    if(p1.ready_t<p2.ready_t||(p1.ready_t==p2.ready_t&&p1.exec_t<p2.exec_t)){
        return 1;
    }else{
        return 0;
    }
}

void QsortReady(Process* p,int N){ //sort with large to small
    if(N==1){
        return;
    }
    int newPivot = Partition(p,N);
    if(newPivot>1){
        QsortReady(p,newPivot);
    }
    if(N-newPivot-1>1){
        QsortReady(&p[newPivot+1],N-newPivot-1);
    }

}
int Partition(Process* p,int N){
    int pivot_idx = 0;
    for(int i=1;i<N;i++){
        if(earlierP(p[i],p[pivot_idx])){ //p[i] > p[pivot_idx]
            for(int j=i;j>pivot_idx;j--){
                swap(&p[j],&p[j-1]);
                //printf("Swap\n");
            }
            pivot_idx++;
        }
    }
    //printf("pivot_idx:%d\n",pivot_idx );
    return pivot_idx;
}
void swap(Process* p1,Process* p2){
    Process* tmp = (Process*)malloc( sizeof(Process));
    *tmp = *p1;
    *p1 = *p2;
    *p2 = *tmp;
}