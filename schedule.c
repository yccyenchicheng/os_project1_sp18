#include <sched.h>
#include <stdio.h>
#include <string.h>

struct Process {
    


}



int main(int argc, char *argv[]) {
    
    /*
    FILE* fp = argv[1];
    if (! fp.isopen() ) {
        fprintf(stderr, "File open error");
    }*/
    
    char schedule_policy[10];
    int N;
    char p_name_buff[32];
    int ready_t;
    int exec_t;

    fscanf(stdin, "%s\n", schedule_policy);
    fscanf(stdin, "%d\n", &N);
    
    fprintf(stdout, "%s\n", schedule_policy);
    fprintf(stdout, "%d\n", N);
    int i;
    for (i = 0; i < N; ++i) {
        fscanf(stdin, "%s %d %d\n", p_name_buff, &ready_t, &exec_t);

        fprintf(stdout, "%s %d %d\n", p_name_buff, ready_t, exec_t);
    }

    printf("Hello world!\n");
    return 0;
}

