#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>
//#include <linux/time.h>

int main(int argc, char** argv) {
	
        struct timespec ts;
        syscall(335, &ts);
        printf("Invoking Sys call 335. Current time is: %lu.%09lu\n", ts.tv_sec, ts.tv_nsec);

}
