#ifndef _UTIL_H_
#define _UTIL_H_

#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <wait.h>
#include <assert.h>

#define BUFF_SIZE 32

/* Program to implement a queue using two stacks */
typedef struct {
    char p_name[BUFF_SIZE];  
    int ready_t;
    int exec_t;
    pid_t pid;
} Process;

typedef struct {
   int current_size;
   int max_size;
   Process* container;
} myHeap;

int myPush(myHeap*, Process*);

int myPop(myHeap*);

int myFront(myHeap*, Process*); 

int myEmpty(myHeap*);


void print(Process p);

void unit_time();

int str_equal(char* c1,char* c2);

void child_execution(struct sched_param sch_p, Process current_p, struct timespec ts_start, struct timespec ts_end);

void swap(Process* p1,Process* p2);

int largerP(Process p1,Process p2);

void ToHeap(Process* p,int N);

void MaxHeapify(Process* p,int N,int index);

int Partition(Process* p,int N);

void QsortReady(Process* p,int N);

void printHeap(Process* p,int N);

struct sNode
{
     Process data;
     struct sNode *next;
};

void push(struct sNode** top_ref, Process new_data);

Process pop(struct sNode** top_ref);

struct queue
{
    Process front;
    Process end;
    struct sNode *stack1;
    struct sNode *stack2;
};

void enQueue(struct queue *q, Process x);

Process deQueue(struct queue *q);


#endif
