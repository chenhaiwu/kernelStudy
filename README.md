# KernelStudy
用于研究linux kernel的多模块实现

[2020-5-17]
EN: add a new tool for access physical address from user space.
CH: 创建一个内核模块dram，用来响应用户态的请求，再创建一个文件fileview编写，用于人机交互。
1：增加一个内核模块dram，用于响应用户态的访问请求.
2：增加一个用户态工具fileview，用于访问显示物理内存的值.
3：使用过程中，要先创建一个设备, mknod /dec/dram -c 85 0

[2020-5-17]
EN: add new kernel module for studying the kernel vaddr transfor to physical address process.
CH: 增加内核模块，用于观察内核虚拟地址如何逐级转化成物理地址。(cr3->pgd->p4d(0)->pud->pmd->pte->paddr)
[功能] 增加内核模块用于观察虚拟地址转化成物理地址
[详细] 
1：打印内核的mm的相关宏，如gpd/pud/pmd/pte等偏移。
2：打印cr0/cr3段基址寄存器。
3：虚拟地址转化物理地址过程