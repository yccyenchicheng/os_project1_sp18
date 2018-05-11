#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>

#include <linux/types.h>
//#include <unistd.h>


asmlinkage void sys_print_ns_time(char *tag, pid_t pid, struct timespec64 *ts_start, struct timespec64 *ts_end)
{
        printk("%s %d %lu %lu\n", tag, pid, ts_start->tv_nsec, ts_end->tv_nsec);
        return;
}

