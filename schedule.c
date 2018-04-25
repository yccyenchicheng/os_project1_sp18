#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"


#define DEBUG 1

typedef struct List {
	Process*	p;
	struct List* next;
	struct List* prev;
} List;


void print(Process p) {
    printf("%s %d %d\n", p.p_name, p.ready_t, p.exec_t);
}

void unit_time() {
    volatile unsigned long i;
    for (i = 0; i < 1000000UL; i++);
}
//void Insert(List* front, List* insert_element);
//void InsertionOrder(List* first_element, List* curr, List* insert);

int main(int argc, char *argv[]) {
    
    /*
    FILE* fp = argv[1];
    if (! fp.isopen() ) {
        fprintf(stderr, "File open error");
    }*/
    
    char schedule_policy[10];
    int N;
    char p_name_buff[BUFF_SIZE];
    int ready_t;
    int exec_t;

	List* first_element = NULL;

    fscanf(stdin, "%s\n", schedule_policy);
    fscanf(stdin, "%d\n", &N);
   
    Process p_arr[N];
	List	l_arr[N];
    List* curr;

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
		l_arr[i].p = &(p_arr[i]);
        printf("checking l_arr: ");
        print(*(l_arr[i].p));
        if(&l_arr[i] == NULL)
            printf("11111\n");
	//	InsertionOrder(&first_element, first_element, &l_arr[i]);
        #ifdef DEBUG
        printf("i = %d, p_name = %s, p_name's address: %p\n", 
                i, p_arr[i].p_name, (p_arr[i].p_name));
        #endif
    }

    curr = first_element;
    if (curr == NULL)
        printf("123\n");
    while(curr != NULL)
    {
        printf("456\n");
        printf("Now process name is %s", curr->p->p_name);
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
    int pid[N];

    printf("test insert\n");
    List* head = NULL;
    head = malloc(sizeof(List));

    head->p = &(p_arr[0]);
    print(*(head->p));

    List* next = (List*) malloc(sizeof(List));
    next->p = &(p_arr[1]);
    //Insert (head, next);

    curr = head;
    while(curr != NULL)
    {
        printf("Test\n");
        printf("Now process name is %s\n", curr->p->p_name);
        curr = curr->next;
    }

    /* Create a queue with items 1 2 3*/
    struct queue *q = (struct queue*)malloc(sizeof(struct queue));
    q->stack1 = NULL;
    q->stack2 = NULL;
    enQueue(q, p_arr[4]);
    enQueue(q, p_arr[1]);
    enQueue(q, p_arr[3]);
     
    /* Dequeue items */
    /*
    printf("%d ", deQueue(q));
    printf("%d ", deQueue(q));
    printf("%d \n", deQueue(q));
    */
    printf("testing queue\n");
    print(deQueue(q));	
    print(deQueue(q));  
    print(deQueue(q));  

    return 0;
}






/*
void Insert(List* front, List* insert_element)
{
	if(front->next !=NULL){ 
		insert_element->next	= front->next;
		(front->next)->prev = insert_element;
	}
	front->next = insert_element;
	insert_element->prev = front;
}

void InsertionOrder(List* first_element, List* curr, List* insert)
{
    List* tmp;
	if ( *first_element == NULL)
	{
        //printf("in\n");
        //tmp = &first_element;
		*first_element = insert;
        //insert = tmp;
	}
	else
	{
		if((curr->next != NULL) && ((curr->next)->p->ready_t <= insert->p->ready_t))
		{
			curr = curr->next;
			InsertionOrder(first_element, curr, insert);
		}
		else
		{
			Insert(curr, insert);
		}
	}
}*/

