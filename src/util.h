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
#define PRINT_INTERVAL 250
/* Program to implement a queue using two stacks */
//extern struct Process;
typedef struct {
    char p_name[BUFF_SIZE];  
    int ready_t;
    int exec_t;
    pid_t pid;
} Process;
void print(Process p);
void unit_time();
int str_equal(char* c1,char* c2);
void child_execution(struct sched_param sch_p,Process currentP);
void swap(Process* p1,Process* p2);
int largerP(Process p1,Process p2);
void ToHeap(Process* p,int N);
void MaxHeapify(Process* p,int N,int index);
int Partition(Process* p,int N);
void QsortReady(Process* p,int N);



// /* structure of a stack node */
// struct sNode
// {
//     //int data;
//     Process data;
//     struct sNode *next;
// };
 
// /* Function to push an item to stack*/
// //void push(struct sNode** top_ref, int new_data);
// void push(struct sNode** top_ref, Process new_data);
 

// /* Function to pop an item from stack*/
// Process pop(struct sNode** top_ref);
 
// /* structure of queue having two stacks */
// struct queue
// {
//     struct sNode *stack1;
//     struct sNode *stack2;
// };
 
// /* Function to enqueue an item to queue */
// void enQueue(struct queue *q, Process x)
// {
//     push(&q->stack1, x);
// }
 
// /* Function to dequeue an item from queue */
// Process deQueue(struct queue *q)
// {
//     Process x;
//     /* If both stacks are empty then error */
//     if(q->stack1 == NULL && q->stack2 == NULL)
//     {
//         printf("Q is empty");
//         getchar();
//         exit(0);
//     }
 
// /* Move elements from stack1 to stack 2 only if
// stack2 is empty */
// if(q->stack2 == NULL)
// {
//     while(q->stack1 != NULL)
//     {
//         x = pop(&q->stack1);
//         push(&q->stack2, x);
         
//     }
// }
 
// x = pop(&q->stack2);
// return x;
// }
 
// /* Function to push an item to stack*/
// void push(struct sNode** top_ref, Process new_data)
// {
//     /* allocate node */
//     struct sNode* new_node =
//         (struct sNode*) malloc(sizeof(struct sNode));
//         if(new_node == NULL)
//         {
//             printf("Stack overflow \n");
//             getchar();
//             exit(0);
             
//         }
 
// /* put in the data */
// new_node->data = new_data;
 
// /* link the old list off the new node */
// new_node->next = (*top_ref);
 
// /* move the head to point to the new node */
// (*top_ref) = new_node;
// }
 
// /* Function to pop an item from stack*/
// Process pop(struct sNode** top_ref)
// {
//     Process res;
//     struct sNode *top;
     
//     /*If stack is empty then error */
//     if(*top_ref == NULL)
//     {
//         printf("Stack overflow \n");
//         getchar();
//         exit(0);
         
//     }
//     else
//     {
//         top = *top_ref;
//         res = top->data;
//         *top_ref = top->next;
//         free(top);
//         return res;
         
//     }
// }

// void unit_time() {
//     volatile unsigned long i;
//     for(i = 0; i < 1000000UL; i++);
// }


#endif