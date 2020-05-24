# KernelStudy
用于研究linux kernel的多模块实现

[2020-5-24] add a share irq with tasklet handler.
CH: 
1：增加一个内核模块，共享一根中断线，用于调试了解中断申请及释放的接口。
2：中断可以分为上下半部，下半部使用tasklet来处理。

[2020-5-18] add a monitor to watch cpu loading by uptime.
CH: 
1：增加一个内核模块，使用uptime中的一个全局变量，取出全局变量表示1分钟的负载。
2：负载大于3，则打印所有进程的所有的task信息。
3：创建一个hrtimer的定时器，使用到时的方式，每隔10ms检查一下负载信息。

[2020-5-17]
EN: add a kernel moduel to print task_struct key info.
CH: 
1：增加一个内核模块task_struct，用于显示进程的重要信息字段。
2：内核基本list_head for each的使用。

[2020-5-17]
EN: add a new tool for access physical address from user space.
CH: 创建一个内核模块dram，用来响应用户态的请求，再创建一个文件fileview编写，用于人机交互.
1：增加一个内核模块dram，用于响应用户态的访问请求.
2：增加一个用户态工具fileview，用于访问显示物理内存的值.
3：使用过程中，要先创建一个设备, mknod /dec/dram -c 85 0

[2020-5-17]
EN: add new kernel module for studying the kernel vaddr transfor to physical address process.
CH: 增加内核模块，用于观察内核虚拟地址如何逐级转化成物理地址。(cr3->pgd->p4d(0)->pud->pmd->pte->paddr)
[功能] 增加内核模块用于观察虚拟地址转化成物理地址
[详细]
1：打印内核的mm的相关宏，如gpd/pud/pmd/pte等偏移
2：打印cr0/cr3段基址寄存器
3：虚拟地址转化物理地址过程