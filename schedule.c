#include <sched.h>
#include <stdio.h>
#include <string.h>

#define DEBUG 1
#define BUFF_SIZE 32

typedef struct {
    char p_name[BUFF_SIZE];  
    int ready_t;
    int exec_t;
	int pid;
} Process;

struct List {
	Process*	p;
	struct List* next;
	struct List* prev;
};


void print(Process p) {
    printf("%s %d %d\n", p.p_name, p.ready_t, p.exec_t);
}

void unit_time() {
    volatile unsigned long i;
    for (i = 0; i < 1000000UL; i++);
}
void Insert(struct List* front, struct List* insertelement);
void Insertinorder(struct List* firstelement, struct List* curr, struct List* insert);

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
	struct List* firstelement = NULL;
	

    fscanf(stdin, "%s\n", schedule_policy);
    fscanf(stdin, "%d\n", &N);
   
    Process p_arr[N];
	struct List	l_arr[N];

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
		Insertinorder(firstelement, firstelement, &l_arr[i]); 
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
    int pid[N];


	

    return 0;
}



void Insert(struct List* front, struct List* insertelement)
{
	if(front->next !=NULL){ 
		insertelement->next	= front->next;
		(front->next)->prev = insertelement;
	}
	front->next = insertelement;
	insertelement->prev = front;
}

void Insertinorder(struct List* firstelement, struct List* curr, struct List* insert)
{
	if(firstelement == NULL)
	{
		firstelement = insert;
	}
	else
	{
		if((curr->next != NULL) && ((curr->next)->p->ready_t <= insert->p->ready_t))
		{
			curr = curr->next;
			Insertinorder(firstelement, curr, insert);
		}
		else
		{
			Insert(curr, insert);
		}
	}
}

