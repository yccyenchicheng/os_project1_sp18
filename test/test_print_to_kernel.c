#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <time.h>

int main(int argc, char** argv) {
	
        char* tag = "[Project 1]";
        pid_t pid = getpid(); 
        struct timespec ts_start;
        struct timespec ts_end;

        syscall(335, &ts_start); // getting ts_start
        syscall(335, &ts_end);

        printf("Invoking Sys call 335. ts_start is: %lu.%09lu\n", ts_start.tv_sec, ts_start.tv_nsec);
        printf("Invoking Sys call 335. ts_end is: %lu.%09lu\n", ts_end.tv_sec, ts_end.tv_nsec);

        syscall(334, tag, pid, &ts_start, &ts_end); 

}
