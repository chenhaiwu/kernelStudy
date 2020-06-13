# KernelStudy
用于研究linux kernel的多模块实现

[2020-6-9] [list] add a module of list operating.
CH: 
1：增加一个内核模块，专用于学习list的操作，包括add/modify/del/遍历。


[2020-5-31] [tools] add a module for rmmod put one module to release the module.
CH: 
1：增加一个小工具，用于+-内核模块的引用计数。
2: cat /proc/kallsyms | grep [^_]modules$
3: module_put/module_get

[2020-5-31] [memory] add a module for mmap and r/w from userspace.
CH: 
1：增加一个内核模块，初始化时申请一块vmalloc内存，（内核虚拟地址、物理地址、pfn的转换，vmalloc_to_page的各种转化）。
2：设置page reserved，不让swap出去。
3: 注册一个字符设备mknod /dev/my_memmap_dev -c 240 0，并在初始化时register_chrdev
4: 打开字符设置，并设置mmap的方法vm_operations_struct my_memmap_vm_ops。
5：在vmf中实现个人的fault函数：入参vmf_fault，中有用户态地址：vmf->address，可以与内核申请的地址做offset, vmf->pgoff，是个整数表示在整段中处于第几页。
   通过这些可以求出pfn, kaddr, 等。
6：在/proc/pid/maps/中可以看到对应的内存映射。

[2020-5-24] [irq] add a share irq with tasklet handler.
CH: 
1：增加一个内核模块，共享一根中断线，用于调试了解中断申请及释放的接口。
2：中断可以分为上下半部，下半部使用tasklet来处理。

[2020-5-18] [task/irq] add a monitor to watch cpu loading by uptime.
CH: 
1：增加一个内核模块，使用uptime中的一个全局变量，取出全局变量表示1分钟的负载。
2：负载大于3，则打印所有进程的所有的task信息。
3：创建一个hrtimer的定时器，使用到时的方式，每隔10ms检查一下负载信息。

[2020-5-17] [task]
EN: add a kernel moduel to print task_struct key info.
CH: 
1：增加一个内核模块task_struct，用于显示进程的重要信息字段。
2：内核基本list_head for each的使用。

[2020-5-17] [memory/tools]
EN: add a new tool for access physical address from user space.
CH: 创建一个内核模块dram，用来响应用户态的请求，再创建一个文件fileview编写，用于人机交互.
1：增加一个内核模块dram，用于响应用户态的访问请求.
2：增加一个用户态工具fileview，用于访问显示物理内存的值.
3：使用过程中，要先创建一个设备, mknod /dec/dram -c 85 0

[2020-5-17] [memory]
EN: add new kernel module for studying the kernel vaddr transfor to physical address process.
CH: 增加内核模块，用于观察内核虚拟地址如何逐级转化成物理地址。(cr3->pgd->p4d(0)->pud->pmd->pte->paddr)
[功能] 增加内核模块用于观察虚拟地址转化成物理地址
[详细]
1：打印内核的mm的相关宏，如gpd/pud/pmd/pte等偏移
2：打印cr0/cr3段基址寄存器
3：虚拟地址转化物理地址过程