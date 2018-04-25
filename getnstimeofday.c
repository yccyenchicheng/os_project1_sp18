#include <stdio.h>
#include <linux/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    struct timespec ts_start,ts_end,test_of_time;
   
    
    getnstimeofday(&ts_start);
    printk("mkdir being hijacked\n");
    //printk("pid is %d ", current->pid);
    printk("call #: 39 \n");
    //printk("user_id of process: %d, effuid: %d\n\n", current->uid, current->euid);
    getnstimeofday(&ts_end);
    test_of_time = timespec_sub(ts_end,ts_start);
    printk("%lu", test_of_time.tv_nsec);
    return val;

    return 0;
}
