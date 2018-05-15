#include <stdio.h>

typedef struct {
	char name[20];
	int ready_t;
	int exec_t;
} Process;
typedef struct {
	int next;
	int prev;
} Node;


void FIFO_handler(Process* p_arr, int N);
void RR_handler(Process* p_arr, int N);
void SJF_handler(Process* p_arr, int N);
void PSJF_handler(Process* p_arr, int N);
void InsertBack(Node* List, int* head, int i);

void InsertSorted(Node* List, int* head, int i, int* prior);
void PrintList(Node* List, int head);

void ReadyList(Node* List, int* head, int* prior, int N);
int main()
{
	char policy[10];
	int N;
	scanf("%s\n",policy);
	scanf("%d\n",&N);
	policy[9] = 0;
	Process p_arr[N];

	for(int i=0; i<N; i = i+1)
	{
		scanf("%s %d %d\n", p_arr[i].name, &p_arr[i].ready_t, &p_arr[i].exec_t);
	}
	for(int i=0; i<N; i = i+1)
	{
		printf("%s %d %d\n", p_arr[i].name, p_arr[i].ready_t, p_arr[i].exec_t);
	}


	if(policy[0] == 'F'){
		printf("FIFO %d\n", N);
		FIFO_handler(p_arr, N);
	} else if(policy[0] == 'R'){
		printf("RR %d\n",N);
		RR_handler(p_arr, N);
	} else if(policy[0] == 'S'){
		printf("SJF %d\n",N);
		SJF_handler(p_arr,N);
	} else if(policy[0] == 'P'){
		printf("PSJF %d\n",N);
		PSJF_handler(p_arr,N);
	}

	 

	return 0;
}

//Insert i-th element to priority List with respect to prior
//When we construct ready list, prior is the ready time
//(ready list is the list sorted by ready_t, not the list already ready)
//When insert execution list of SJF and PSJF, priority is execition time
//(execution list is the list consist of process that ready and sort by priority)
void InsertSorted(Node* List, int* head, int i, int* prior)
{
	if(*head == -1){
		*head = i;
	}
	else{
		int cur = *head;
		while(List[cur].next != -1){
			int next = List[cur].next;
			if(prior[i] < prior[next]){
				break;
			}
			else{
				cur = next;
			}
		}
		if(prior[i]<prior[*head]){
			List[i].next = *head;
			List[*head].prev = i;
			*head = i;
		}
		else{
			int next = List[cur].next;
			if(next != -1){
				List[next].prev = i;
				List[i].next = next;
			}
			List[cur].next = i;
			List[i].prev = cur;
		}

	}
}
//While RR, we need to insert from back
void InsertBack(Node* List, int* head, int i)
{
	int cur = *head;
	while(cur != -1 && List[cur].next != -1){
		cur = List[cur].next;
	}
	if(cur == -1){
		*head = i;	
	}
	else{
		List[cur].next = i;
		List[i].prev = cur;
	}
	//printf("AFTER INSERT: \n");
	//printf("head is %d\n", *head);
	//PrintList(List, *head);
}
void PrintList(Node* List, int head)
{
	int cur = head;
	printf("  List: ");
	while(cur != -1){
		printf("-%d-", cur);
		cur = List[cur].next;
	}
	printf("\n");
}

void ReadyList(Node* List, int* head, int* prior, int N)
{
	for(int i=0; i<N; i=i+1){
		printf("Insert sorted %d\n", i);
		InsertSorted(List, head, i, prior);
		printf("	");
		PrintList(List, *head);
	}
}
void Delete(Node* List, int* head)
{
	int temp = *head;
	*head = List[temp].next;
	List[temp].next = -1;
	List[*head].prev = -1;
}
void FIFO_handler(Process* p_arr, int N)
{
	Node List_r[N];
	int head_r = -1;
	int prior_r[N];
	Node List_e[N];
	int head_e = -1;
	int prior_e[N];
	for(int i=0; i<N; i++){
		List_r[i].next = -1;
		List_r[i].prev = -1;
		List_e[i].next = -1;
		List_e[i].prev = -1;
		prior_r[i] = p_arr[i].ready_t;
		prior_e[i] = p_arr[i].exec_t;
	}
	printf("Call Ready:\n");
	ReadyList(List_r, &head_r, prior_r, N);
	int time = 0;
	while(head_r != -1 || head_e != -1){
		while(head_r != -1 && prior_r[head_r]<=time){
			//printf("p%d is ready at %d\n", head_r, time);
			printf("%s is ready at %d\n", p_arr[head_r].name, p_arr[head_r].ready_t);
			InsertSorted(List_e,&head_e, head_r, prior_r);
			Delete(List_r, &head_r);
		}
		while(head_e!=-1){
			//printf("p%d is terminated at %d\n", head_e, time + prior_e[head_e]-1);
			printf("%s is terminated at %d\n", p_arr[head_e].name, time + prior_e[head_e]-1);
			time = time + prior_e[head_e];
			Delete(List_e, &head_e);
		}
		if(head_e==-1 && prior_r[head_r]>time){
			time = prior_r[head_r];
		}
	}
}
void RR_handler(Process* p_arr, int N)
{
	Node List_r[N];
	int head_r = -1;
	int prior_r[N];
	Node List_e[N];
	int head_e = -1;
	int prior_e[N];
	for(int i=0; i<N; i = i+1)
	{
		prior_r[i] = p_arr[i].ready_t;
		prior_e[i] = p_arr[i].exec_t;
		List_r[i].next = -1;
		List_r[i].prev = -1;
		List_e[i].next = -1;
		List_e[i].prev = -1;
	}	
	printf("Call Ready:\n");
	ReadyList(List_r, &head_r, prior_r, N);
	int time = 0;
	while(head_r != -1 || head_e != -1)
	{
		while(head_r != -1 && prior_r[head_r]<=time){
			//printf("p%d ready at %d\n",head_r,time);
			printf("%s is ready at %d\n", p_arr[head_r].name, p_arr[head_r].ready_t);
			InsertBack(List_e, &head_e, head_r);
			Delete(List_r, &head_r);
		}
		if(head_e != -1){
			if(prior_e[head_e]<=500)
			{
				//printf("p%d terminated at %d\n", head_e, time + prior_e[head_e]-1);
                                printf("%s is terminated at %d\n", p_arr[head_e].name, time + prior_e[head_e]-1);
				time = time + prior_e[head_e];
				Delete(List_e, &head_e);
			}
			else{
				time += 500;
				prior_e[head_e] = prior_e[head_e] - 500;
				int temp = head_e;
				while(head_r != -1 && prior_r[head_r]<=time){
					//printf("p%d is ready at %d\n", head_r, time);
                                        printf("%s is ready at %d\n", p_arr[head_r].name, p_arr[head_r].ready_t);
					InsertBack(List_e, &head_e, head_r);
					Delete(List_r, &head_r);
				}
				Delete(List_e, &head_e);
				InsertBack(List_e, &head_e, temp);
			}
		}
		if(head_e ==-1 && (head_r != -1) && prior_r[head_r]>time){
			time = prior_r[head_r];
		}
	}

}
void SJF_handler(Process* p_arr, int N)
{
	Node List_r[N];
	int prior_r[N];
	int head_r=-1;
	Node List_e[N];
	int prior_e[N];
	int head_e=-1;
	for(int i=0; i<N; i++){
		prior_r[i] = p_arr[i].ready_t;
		prior_e[i] = p_arr[i].exec_t;
		List_r[i].next = -1;
		List_r[i].prev = -1;
		List_e[i].next = -1;
		List_e[i].prev = -1;
	}
	printf("Call Ready\n");
	ReadyList(List_r, &head_r, prior_r, N);
	int time = 0;
	while(head_r != -1 || head_e != -1){
		while(head_r != -1 && prior_r[head_r]<=time){
		//	printf("p%d is ready at %d", head_r, time);
			printf("%s is ready at %d\n", p_arr[head_r].name, p_arr[head_r].ready_t);
			InsertSorted(List_e, &head_e, head_r, prior_e);
			Delete(List_r, &head_r);
		}
		if(head_e != -1){
			//printf("p%d is terminated at %d", head_e, time+prior_e[head_e]-1);
                        printf("%s is terminated at %d\n", p_arr[head_e].name, time + prior_e[head_e]-1);
			time = time + prior_e[head_e];
			Delete(List_e, &head_e);
		}
		if(head_e == -1 && head_r != -1 && prior_r[head_r]>time){
			time = prior_r[head_r];
		}

	}

}
void PSJF_handler(Process* p_arr, int N)
{
	Node List_r[N];
	int head_r = -1;
	int prior_r[N];
	Node List_e[N];
	int head_e= -1;
	int prior_e[N];
	for(int i =0; i<N; i=i+1){
		List_r[i].next = -1;
		List_r[i].prev = -1;
		List_e[i].next = -1;
		List_e[i].prev = -1;
		prior_r[i] = p_arr[i].ready_t;
		prior_e[i] = p_arr[i].exec_t;
	}
	printf("Call Ready\n");
	ReadyList(List_r, &head_r, prior_r, N);
	int time = 0;
	while(head_r != -1 || head_e != -1){
		while(head_r != -1 && prior_r[head_r] <= time){
		//	printf("p%d is ready at %d\n", head_r, time);
			printf("%s is ready at %d\n", p_arr[head_r].name, p_arr[head_r].ready_t);
			InsertSorted(List_e, &head_e, head_r, prior_e);
			printf("	");
			PrintList(List_e, head_e);
			Delete(List_r, &head_r);
		}
		if(head_e != -1){
			prior_e[head_e] = prior_e[head_e] - 1;
			if(prior_e[head_e]<=0){
		//		printf("p%d is terminated at %d\n",head_e, time);
                       		printf("%s is terminated at %d\n", p_arr[head_e].name, time);
				Delete(List_e, &head_e);
				
			}
			time = time + 1;
		}
		if(head_e == -1 && head_r != -1 && prior_r[head_r]>time){
			time = prior_r[head_r];
		}
	}
}




