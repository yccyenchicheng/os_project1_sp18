
typedef struct List {
	Process*	p;
	struct List* next;
	struct List* prev;
} List;


//void Insert(List* front, List* insert_element);
//void InsertionOrder(List* first_element, List* curr, List* insert);

int main(int argc, char *argv[])
{
    
    curr = first_element;
    if (curr == NULL)
        printf("123\n");
    while(curr != NULL)
    {
        printf("456\n");
        printf("Now process name is %s", curr->p->p_name);
    }
    
    
    curr = head;
    while(curr != NULL)
    {
        printf("Test\n");
        printf("Now process name is %s\n", curr->p->p_name);
        curr = curr->next;
    }
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

