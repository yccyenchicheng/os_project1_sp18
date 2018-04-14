#include <sched.h>
#include <stdio.h>
#include <string.h>

//#define DEBUG 1

struct Process {
    char* p_name;  
    int ready_t;
    int exec_t;
};

void print(struct Process p) {
    printf("%s %d %d\n", p.p_name, p.ready_t, p.exec_t);
}

int main(int argc, char *argv[]) {
    
    /*
    FILE* fp = argv[1];
    if (! fp.isopen() ) {
        fprintf(stderr, "File open error");
    }*/
    
    char schedule_policy[10];
    int N;
    char* p_name;
    char p_name_buff[32];
    int ready_t;
    int exec_t;

    fscanf(stdin, "%s\n", schedule_policy);
    fscanf(stdin, "%d\n", &N);
   
    struct Process p_arr[N];

    #ifdef DEBUG
    fprintf(stdout, "%s\n", schedule_policy);
    fprintf(stdout, "%d\n", N);
    #endif

    int i;
    for (i = 0; i < N; ++i) {
        //char p_name_buff[32];
        //fscanf(stdin, "%s %d %d\n", p_arr[i].p_name, &(p_arr[i].ready_t),
        //                            &(p_arr[i].exec_t));
        
        fscanf(stdin, "%s %d %d\n", p_name_buff, &ready_t,
                                    &exec_t);

        //strcpy(p_name, p_name_buff); 
        struct Process tmp_p = {p_name, ready_t, exec_t}; 
        p_arr[i] = tmp_p;
        strcpy(p_arr[i].p_name, p_name_buff);
        #ifdef DEBUG
        print(tmp_p);
        //print(&p_arr[i]);
        printf("%p\n", &(p_arr[i].p_name));
        printf("%s\n", (p_arr[i].p_name));
        printf("%p\n", &(p_name_buff));
        #endif
    }

    p_arr[1].p_name = "changed";
   
    #ifndef DEBUG
    i = 0;
    printf("checking value\n");
    for (i = 0; i < N; ++i) {
        printf("%d ", i);
        print(p_arr[i]);
        printf("%p\n", &(p_arr[i]));
    }
    #endif   
    
    #ifdef DEBUG 
    i = 1;
    printf("%s %d %d\n", p_arr[i].p_name, p_arr[i].ready_t, p_arr[i].exec_t);    
    printf("%p\n", &(p_arr[i].p_name));
    #endif

    return 0;
}

