# KernelStudy
用于研究linux kernel的多模块实现

[2020-5-17]
EN: add new kernel module for studying the kernel vaddr transfor to physical address process.
CH: 增加内核模块，用于观察内核虚拟地址如何逐级转化成物理地址。(cr3->pgd->p4d(0)->pud->pmd->pte->paddr)