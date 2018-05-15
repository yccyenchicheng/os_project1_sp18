#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <wait.h>
#include <assert.h>

/* for system call */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <time.h>
#include <sys/syscall.h>

#include "util.h"

/* Program to implement a queue using two stacks */
void print(Process p) {
    printf("%s %d %d %d\n", p.p_name,p.pid, p.ready_t, p.exec_t);
}

void unit_time() {
    volatile unsigned long i;
    for(i = 0; i < 1000000UL; i++);
}

int str_equal(char* c1,char* c2){
    int i = 0;
    while(c1[i]==c2[i]){

        if(c1[i]=='\0'){
            return 1;
        }
        i++;
    }
    return 0;

}

void child_execution(struct sched_param sch_p, Process current_p, struct timespec ts_start, struct timespec ts_end){
    char *tag = "[Project1]";
    int total_time = 0;
    int exec_t = current_p.exec_t;
    pid_t cpid = getpid();

    for(int j = 0; j < exec_t - 1; ++j)
    {
        //printf("child pid: %d, child's time counter: %d\n", cpid, j);
        unit_time();
        sch_p.sched_priority = 2;


        //printf("### change to parent ###\n");
        assert(sched_setscheduler(cpid, SCHED_FIFO, &sch_p) != -1); // return control to parent
        ++total_time;
    }
    /* last unit of time */
    //printf("child pid: %d, child's time counter: %d\n", cpid, total_time);
    unit_time();     
    syscall(335, &ts_end); // for printk
    ++total_time;

    //printf("%s, pid: %d is about to exit!. time: %d\n", current_p.p_name, getpid(), total_time);

    /* should print p_name, pid when it finishs the execution */
    printf("%s %d\n", current_p.p_name, cpid);
    /* for dmesg */
    syscall(334, tag, cpid, &ts_start, &ts_end); // for dmesg

    _exit(0);
}

void ToHeap(Process* p,int N){
    if(N==1){
        return;
    }
    for(int i=(N>>1)-1;i>=0;i--){
        MaxHeapify(p,N,i);
    }
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
    if(p1.exec_t<p2.exec_t||(p1.exec_t==p2.exec_t&&p1.ready_t<p2.ready_t)){
        return 1;
    }else{
        return 0;
    }
}
int earlierP(Process p1,Process p2){ //return true if ready_t p1 < p2 
    if(p1.ready_t < p2.ready_t || (p1.ready_t == p2.ready_t && ( strcmp(p1.p_name, p2.p_name) < 0 ))){
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
            }
            pivot_idx++;
        }
    }
    return pivot_idx;
}

void swap(Process* p1,Process* p2){
    Process* tmp = (Process*)malloc( sizeof(Process));
    *tmp = *p1;
    *p1 = *p2;
    *p2 = *tmp;
}

void printHeap(Process* p,int N){
    for(int i=0;i<N;i++){
        print(p[i]);
    }
}


/* Function to enqueue an item to queue */
void enQueue(struct queue *q, Process x)
{
    // set up front
    if (q->stack1 == NULL && q->stack2 == NULL) {
        q->front = x;
    }
    q->end = x;
    push(&q->stack1, x);
}

/* Function to dequeue an item from queue */
Process deQueue(struct queue *q)
{
    Process x;
    /* If both stacks are empty then error */
    if(q->stack1 == NULL && q->stack2 == NULL)
    {
        printf("Q is empty");
        getchar();
        exit(0);
    }

    /* Move elements from stack1 to stack 2 only if
       stack2 is empty */
    if(q->stack2 == NULL)
    {
        while(q->stack1 != NULL)
        {
            x = pop(&q->stack1);
            push(&q->stack2, x);

        }
    }
    
    /* setup front. */
    x = pop(&q->stack2);
    if (q->stack2 != NULL) {
        q->front = q->stack2->data;
    } else {
        if (q->stack1 != NULL) {
            Process tmp;
            while(q->stack1 != NULL)
            {
                tmp = pop(&q->stack1);
                push(&q->stack2, tmp);
                
            }
            q->front = tmp;
        }

    }

    return x;
}

/* Function to push an item to stack*/
void push(struct sNode** top_ref, Process new_data)
{
    /* allocate node */
    struct sNode* new_node =
        (struct sNode*) malloc(sizeof(struct sNode));
    if(new_node == NULL)
    {
        printf("Stack overflow \n");
        getchar();
        exit(0);

    }

    /* put in the data */
    new_node->data = new_data;

    /* link the old list off the new node */
    new_node->next = (*top_ref);

    /* move the head to point to the new node */
    (*top_ref) = new_node;
}

/* Function to pop an item from stack*/
Process pop(struct sNode** top_ref)
{
    Process res;
    struct sNode *top;

    /*If stack is empty then error */
    if(*top_ref == NULL)
    {
        printf("Stack overflow \n");
        getchar();
        exit(0);

    }
    else
    {
        top = *top_ref;
        res = top->data;
        *top_ref = top->next;
        free(top);
        return res;

    }
}



