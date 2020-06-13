#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/err.h>

#define MAX_LIST_N 100
#define MAX_CHAR_N 50
struct my_struct {
    struct list_head list;
    char name[MAX_CHAR_N];
    int pid;
    int id;
};

static LIST_HEAD(myList);

static struct task_struct *my_kernel_task;

int mytask_thread_func(void *data)
{
    static int entry_times = 0;
    while (1) {
        // set_current_state(TASK_UNINTERRUPTIBLE); //这行如何打开，就不会执行到下面的代码的。
        if (kthread_should_stop()) {    // 这一行必须得有，不然在关闭线程时，会导致关不掉，卡死的问题
            printk("kernel should stoop==============: %d, time: %d\n", current->pid, entry_times);
            break;
        }
        printk("kernel thread pid: %d, time: %d\n", current->pid, entry_times);
        entry_times++;
        schedule_timeout(100 * HZ);
    }
    return 0;
}

static int mylistopt_add_all(void)
{
    int i;
    for (i = 0; i < MAX_LIST_N; i++) {
        struct my_struct *p = kzalloc(sizeof(struct my_struct), GFP_KERNEL);
        if (!p) {
            printk("alloc my struct fail\n");
            return -1;
        }
        p->id = i;
        p->pid = current->pid;
        sprintf(p->name, "this is idx %d", p->id);
        list_add_tail(&p->list, &myList);
    }
    return 0;
}

static int mylistopt_print(void)
{
    struct list_head *pos;
    list_for_each(pos, &myList) {
        struct my_struct *pMyobj;
        pMyobj = list_entry(pos, struct my_struct, list);
        printk("id:%d, pid=%d, name:%s\n", pMyobj->id, pMyobj->pid, pMyobj->name);
    }
    return 0;
}

static int mylistopt_del(int id, int del_all)
{
    struct list_head *p, *n;
    list_for_each_safe(p, n, &myList) {
        struct my_struct *pMyobj;
        pMyobj = list_entry(p, struct my_struct, list);
        if (id == pMyobj->id || del_all == 1) {
            list_del(p);
            printk("del: id=%d, name:%s\n", pMyobj->id, pMyobj->name);
            kfree(pMyobj);
        }
    }
    
    return 0;
}

static int __init mylistopt_init(void)
{
    int i, err;
    printk("my list opt entry...\n");
    mylistopt_add_all();
    
    mylistopt_print();

    printk("del ==========================================\n");
    for (i = 0; i < MAX_LIST_N; i++) {
        if (i % 2 == 1) {
            mylistopt_del(i, 0);
        }
    }

    printk("after ========================================\n");
    mylistopt_print();

    my_kernel_task = kthread_create(mytask_thread_func, NULL, "my_test_task1");
    if (IS_ERR(my_kernel_task)) {
        printk("create kernel thread fail\n");
        err = PTR_ERR(my_kernel_task);
        my_kernel_task = NULL;
        return err;
    }
    wake_up_process(my_kernel_task);

    return 0;
}

static void __exit mylistopt_exit(void)
{
    printk("mylistopt_exit ==========================================\n");
    mylistopt_del(0, 1);

    printk("is there anything else ==========================================\n");
    mylistopt_print();

    printk("destroy kernel thread ==========================================\n");
    if (my_kernel_task) {
        printk("del happen, pid=%d\n", my_kernel_task->pid);
        kthread_stop(my_kernel_task);
        my_kernel_task = NULL;
    }
}

module_init(mylistopt_init);
module_exit(mylistopt_exit);

MODULE_LICENSE("GPL");

