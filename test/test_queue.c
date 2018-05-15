#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "../util.h"

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

/*
void idea() {

    int t;
    int total_time, N;
    total_time = 30; // total time for all processes to complete
    N = 10; // assume 10 processes

    // pid_t process_id[N];
    // int next_pid;
    // next_pid = 0;
    // ready_q[N];
    
    for (t = 0; t < total_time; ++t) {
        if (t == ready_q.front().ready_t) {
            process_id[next_pid] = fork();
            
            // sched_setscheduler();
            //
            // write a function to specify how many unit of time
            // the child process should run

            next_pid = next_pid + 1;
        }
        


    }

}*/

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



    /* Create a queue with items 1 2 3*/
    struct queue *q = (struct queue*)malloc(sizeof(struct queue));
    q->stack1 = NULL;
    q->stack2 = NULL;
    enQueue(q, p_arr[4]);
    enQueue(q, p_arr[1]);
    enQueue(q, p_arr[3]);
     
    /* Dequeue items */
    printf("testing queue\n");
    print(deQueue(q));	
    print(deQueue(q));  
    print(deQueue(q));  

    return 0;
}

//Process 

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
