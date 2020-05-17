/*
 * 本文为了了解kernel 进程的结构体信息，由于kernel struct内容特别多，这里关注打印部分重点信息观察 
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/fdtable.h>
#include <linux/fs_struct.h>
#include <linux/mm_types.h>
#include <linux/init_task.h>
#include <linux/types.h>
#include <asm/atomic.h>

static int __init print_pcb(void)
{
    struct task_struct *task, *p;
    struct list_head *pos;
    int cnt = 0;
    printk("begin to print all task: \n");

    // 0号进程的pcb
    task = &init_task;

    list_for_each(pos, &task->tasks) {
        p = list_entry(pos, struct task_struct, tasks);
        cnt++;
        printk("\n\n");
        printk("pid: %d; state: %lx, prio: %d, static prio: %d, normal prio: %d, rt prio: %d, parent id: %d\n",
            p->pid, p->state, p->prio, p->static_prio, p->normal_prio, p->rt_priority, p->parent->pid);
        printk("task name: %s, files count: %d, fs umask: %d, nvcsw: %ld, nivcsw: %ld\n",
            p->comm, atomic_read(&(p->files->count)), p->fs->umask, p->nvcsw , p->nivcsw);
        if (p->mm) {
            printk("mm: 0x%lx, total_vm: %ld \n", p->mm->mmap_base, p->mm->total_vm);
        }
    }

    printk("total task num: %d\n", cnt);
    return 0;
}

static void __exit exit_pcb(void)
{
    printk("exit print tcb.... \n");
}

module_init(print_pcb);
module_exit(exit_pcb);

MODULE_LICENSE("GPL");