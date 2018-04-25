#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <signal.h>

void sighandler(int signum)
{
   printf("pid = %d, Caught signal %d, coming out...\n", getpid(), signum);
   
}
int main(void){
	printf("Parent pid = %d\n",getpid());
	signal(SIGUSR1, sighandler);
	pid_t f=fork();

	if(f>0){
		pause();
		printf("parent running...\n");
		kill(f,SIGUSR1);
		pause();


	}else if(f==0){
		printf("I'm the child process: pid=%d, ppid=%d\n", getpid(), getppid());
		for(int i=0;i<1000000;i++){
			continue;
		}
		kill(getppid(),SIGUSR1);
		pause();
		printf("I'm the child process: pid=%d, ppid=%d\n", getpid(), getppid());
		for(int i=0;i<1000000;i++){
			continue;
		}
		printf("END!\n");
		exit(0);

	}else{
		printf("FORK FAILED\n");
		exit(1);
	}
	printf("Parent pid = %d\n",getpid());
	printf("FINAL!\n");
}