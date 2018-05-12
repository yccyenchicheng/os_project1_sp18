#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/types.h>

// syscall 334
asmlinkage void sys_print_ns_time(char *tag, pid_t pid, struct timespec64 *ts_start, struct timespec64 *ts_end)
{
        printk("%s %d %lu.%09lu %lu.%09lu\n", tag, pid, ts_start->tv_sec, ts_start->tv_nsec, ts_end->tv_sec, ts_end->tv_nsec);
        return;
}

