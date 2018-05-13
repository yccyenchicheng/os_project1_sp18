#ifndef _sjf_H_
#define _sjf_H_

#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "util.h"
#include <signal.h>
#include <wait.h>
#include <assert.h>


//#define PRINT_INTERVAL 250
void sjf_scheduler(Process* p,int N);
#endif