#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>



// register dev
#define MY_MEMMAP_DEV_MAJOR 240
#define MY_MEMMAP_DEV_NAME  "my_memmap_dev"

// mem manage
#define PAGE_CNT 1000
#define MAP_LEN (PAGE_CNT * PAGE_SIZE)
char *s_vmalloc_area;

static void my_memmap_vma_ops_open(struct vm_area_struct *vma);
static void my_memmap_vma_ops_close(struct vm_area_struct *vma);
static vm_fault_t my_memmap_vma_ops_fault(struct vm_fault *vmf);
static int my_memmap_map(struct file *file, struct vm_area_struct *vma);
static int my_memmap_open(struct inode *inoe, struct file *file);

// vma ops
static struct vm_operations_struct my_memmap_vm_ops = {
    .open = my_memmap_vma_ops_open,
    .close = my_memmap_vma_ops_close,
    .fault = my_memmap_vma_ops_fault,
};

// dev ops
static struct file_operations my_memmap_dev_ops = {
    .owner = THIS_MODULE,
    .mmap = my_memmap_map,
    .open = my_memmap_open,
};

static int my_memmap_map(struct file *file, struct vm_area_struct *vma)
{
    unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
    unsigned long size = vma->vm_end - vma->vm_start;

    if (size > MAP_LEN) {
        printk("size is too big\n");
        return -ENXIO;
    }
    if ((vma->vm_flags & VM_WRITE) && !(vma->vm_flags & VM_SHARED)) {
        printk("map: write also request share attr.\n");
        return -EINVAL;
    }
    vma->vm_flags |= VM_FAULT_LOCKED;
    if (offset == 0) {
        vma->vm_ops = &my_memmap_vm_ops;
    } else {
        printk("offset out of range\n");
        return -ENXIO;
    }

    printk("my_memmap_drv_init enry...\n");
    return 0;
}

static int my_memmap_open(struct inode *inoe, struct file *file)
{
    printk("my_memmap_open open, check the order, process name=%s, pid=%d\n", current->comm, current->pid);
    return 0;
}


static void my_memmap_vma_ops_open(struct vm_area_struct *vma)
{
    printk("my_memmap_vma_ops_open open, check the order\n");
}

static void my_memmap_vma_ops_close(struct vm_area_struct *vma)
{
    printk("my_memmap_vma_ops_close close, check the order\n");
}

static vm_fault_t my_memmap_vma_ops_fault(struct vm_fault *vmf)
{
    struct page *page;
    void *page_ptr;
    unsigned long offset, kern_virt_start, pfn_start;

    printk("my_memmap_vma_ops_fault entry...\n");
    offset = vmf->address - vmf->vma->vm_start;
    kern_virt_start = (unsigned long)s_vmalloc_area + (unsigned long)(vmf->pgoff << PAGE_SHIFT);
    pfn_start = (unsigned long)vmalloc_to_pfn((void *)kern_virt_start);

    if ((s_vmalloc_area == NULL) || (vmf->vma == NULL)) {
        printk("vaddr is null\n");
        return VM_FAULT_SIGBUS;
    }
    if (offset > MAP_LEN) {
        return VM_FAULT_SIGBUS;
    }
    page_ptr = s_vmalloc_area + offset;
    page = vmalloc_to_page(page_ptr);
    get_page(page);
    vmf->page = page;
    printk("%s: map kvaddr: 0x%lx (kpaddr: 0x%016lx) to (uaddr: 0x%lx), size: 0x%lx, page: %ld\n",
        __func__, kern_virt_start, pfn_start << PAGE_SHIFT, vmf->address, PAGE_SIZE, vmf->pgoff);
    return 0;
}


static int __init my_memmap_drv_init(void)
{
    int ret;
    unsigned long kern_virt_addr;
    int i;

    printk("my_memmap_drv_init enry...\n");
    ret = register_chrdev(MY_MEMMAP_DEV_MAJOR, MY_MEMMAP_DEV_NAME, &my_memmap_dev_ops);
    if (ret) {
        printk("register my memmap dev fail\n");
        return ret;
    }
    s_vmalloc_area = vmalloc(MAP_LEN);
    kern_virt_addr = (unsigned long)s_vmalloc_area;
    printk("vmalloc 10 pages suc..\n");
    for (i = 0; i < PAGE_CNT; i++) {
        SetPageReserved(vmalloc_to_page((void *)kern_virt_addr));
        sprintf((char *)kern_virt_addr, "test: %d", i);
        kern_virt_addr += PAGE_SIZE;
    }
    printk("s_vmalloc_area at 0x%lx (phys 0x%lx)\n",
        (unsigned long)s_vmalloc_area, (unsigned long)vmalloc_to_pfn((void *)s_vmalloc_area) << PAGE_SHIFT);
    printk("kernel vmalloc(10K) area apply SUC!");

    return 0;
}

static void __exit my_memmap_drv_exit(void)
{
    unsigned long kern_virt_addr;

    printk("my_memmap_drv_exit entry...\n");
    for (kern_virt_addr = (unsigned long)s_vmalloc_area; kern_virt_addr < (unsigned long)s_vmalloc_area + MAP_LEN; kern_virt_addr += PAGE_SIZE) {
        ClearPageReserved(vmalloc_to_page((void *)kern_virt_addr));
    }
    if (s_vmalloc_area) {
        vfree(s_vmalloc_area);
    }
    unregister_chrdev(MY_MEMMAP_DEV_MAJOR, MY_MEMMAP_DEV_NAME);
}

module_init(my_memmap_drv_init);
module_exit(my_memmap_drv_exit);

MODULE_LICENSE("GPL");