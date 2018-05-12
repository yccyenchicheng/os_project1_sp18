#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "util.h"

void fifo(Process*, int);

void rr(Process*, int);

void sjf(Process*, int);

void psjf(Process*, int);

#endif
