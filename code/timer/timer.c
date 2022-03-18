#include <linux/version.h>
#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/kallsyms.h>
#include <linux/sched.h>
#include <linux/tracepoint.h>
#include <linux/stacktrace.h>
#include <linux/sched/task.h>
#include <linux/sched/signal.h>
#include <linux/timer.h>
#include <linux/slab.h>


#define VERSION 2

#if VERSION == 1

static struct timer_list qgpu_timer_heartbeat;

static void qgpu_heartbeat_callback(struct timer_list *timer)
{
	int i = 0;

	for (i = 0; i < 2; i++) {
        printk("data:%lx, i:%d\n", (unsigned long)timer, i);
	}
    // set another timeout 1 sec in the future:
    mod_timer(&qgpu_timer_heartbeat, jiffies + msecs_to_jiffies(1000));
}

static void qgpu_start_heartbeat_timer(void)
{
    printk("start timer p:%lx\n", (unsigned long)&qgpu_timer_heartbeat);
    timer_setup(&qgpu_timer_heartbeat, qgpu_heartbeat_callback, 0);
    mod_timer(&qgpu_timer_heartbeat, jiffies + msecs_to_jiffies(1000));
}

static void qgpu_del_timer(void)
{
    del_timer(&qgpu_timer_heartbeat);
}

#else

static void *p_heartbeat_timer;

static void os_alloc_timerlist(void ** ppTimerlist)
{
	struct timer_list *pTimer = kmalloc(sizeof(struct timer_list), GFP_KERNEL);
	*ppTimerlist = pTimer;
}

static void os_free_timerlist(void *pTimerlist)
{
	kfree(pTimerlist);
}

static void os_timer_setup(void *pTimer, void *callback, unsigned long data)
{
	timer_setup((struct timer_list *)pTimer, callback, data);
}

static int os_mod_timer(void *pTimer, unsigned long millisecond)
{
	return mod_timer((struct timer_list *)pTimer, jiffies + msecs_to_jiffies(millisecond));
}

static int os_del_timer(void *pTimer)
{
	return del_timer((struct timer_list *)pTimer);
}


static void qgpu_heartbeat_callback(void *timer)
{
	int i = 0;

	for (i = 0; i < 2; i++) {
        printk("data:%lx, i:%d\n", (unsigned long)timer, i);
	}
    // set another timeout 1 sec in the future:
    mod_timer(p_heartbeat_timer, jiffies + msecs_to_jiffies(1000));
}

static void qgpu_start_heartbeat_timer(void)
{
	os_alloc_timerlist(&p_heartbeat_timer);
    printk("======start timer p:%lx\n", (unsigned long)p_heartbeat_timer);
	os_timer_setup(p_heartbeat_timer, qgpu_heartbeat_callback, 0);
	os_mod_timer(p_heartbeat_timer, 1);
}

static void qgpu_del_timer(void)
{
    os_del_timer(p_heartbeat_timer);
    os_free_timerlist(p_heartbeat_timer);
}

#endif




static int mytimer_init(void)
{
    printk("mytimer_init entry...\n");

    qgpu_start_heartbeat_timer();

    return 0;
}

static void mytimer_exit(void)
{
    printk("mytimer_exit exit...\n");
    qgpu_del_timer();
}

module_init(mytimer_init)
module_exit(mytimer_exit)

MODULE_LICENSE("GPL v2");
