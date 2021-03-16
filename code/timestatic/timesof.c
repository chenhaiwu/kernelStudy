#include <linux/init.h>
#include <linux/module.h>
#include <linux/fb.h>
#include <linux/dma-mapping.h>
#include <linux/time.h>
#include <linux/timex.h>
#include <linux/rtc.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/module.h>
 
static int __init kernel_time_init(void)
{
    int i;
    struct timeval start_time,end_time;
    long cost_time = 0;   
	          
    printk("kernel time test ...\n");
    do_gettimeofday(&start_time);//获取开始时间
 
	/*延时1s*/
    for(i=0;i<10;i++) {
        mdelay(100);
    }
    do_gettimeofday(&end_time);//获取结束时间
 
    long secsDiff = end_time.tv_sec - start_time.tv_sec;
    cost_time = secsDiff * 1000000
                   + (end_time.tv_usec - start_time.tv_usec);
    printk("Cost time: %ld us\n", cost_time);
 
    return 0;
}
 
static void __exit kernel_time_exit(void)
{
     printk("kernel time exit\r\n");
}
 
module_init(kernel_time_init);
module_exit(kernel_time_exit);
 
MODULE_LICENSE("GPL");
