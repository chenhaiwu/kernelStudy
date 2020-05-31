/*print_module.c*/
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/list.h>
#include<linux/module.h>

MODULE_LICENSE("GPL");

/*
**每次重启内核时都要通过这个命令来获取
**sudo cat /proc/kallsyms | grep [^_]modules$ 
*/
#define MODULES 0xffffffff87cc0f50 
/*在两个修改方案上选择*/
#define DEBUG_MODULE 0 
/*是否启动调试*/
#define DEBUG 1  

static char *name = "memmap";
/*模块参数，传入一个模块的名称*/
module_param(name,charp,S_IRUGO); 

static int __init print_module_init(void) 
{
    struct list_head *module_head;
    struct list_head *pos;
    struct module *p;
    int i= 0;
    int ret;
    printk(KERN_INFO "print module init\n");
    module_head = (struct list_head*)MODULES;
    /*遍历所有的module*/
    list_for_each(pos,module_head) {
        p = list_entry(pos,struct module,list);
#if DEBUG
#if DEBUG_MODULE
/*如果匹配到这个模块，修改状态为living，增加引用计数*/
        if(strcmp(p->name,name)==0){
            p->state = MODULE_STATE_LIVE;
            if((ret = try_module_get(p)) == 0) {
                printk(KERN_ALERT "try_module_get error\n");
            }
            printk(KERN_INFO "%s ret is inc\n",p->name);
        }
#else
/*降低匹配到的模块的引用计数*/
    printk("entry module list ...\n");
        if(strcmp(p->name,name) == 0) {
            module_put(p);
            printk(KERN_INFO "%s ref is dec\n",p->name);
        }
#endif
#endif
    }

    return 0;
}

static void __exit print_module_exit(void)
{
    printk(KERN_INFO "print module exit\n");
}

module_init(print_module_init);
module_exit(print_module_exit);