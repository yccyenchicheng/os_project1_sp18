#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>


asmlinkage void sys_get_ns_time(struct timespec64 *ts)
{
        getnstimeofday64(ts);
        return;
}

