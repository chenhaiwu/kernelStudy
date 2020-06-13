#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>

#define DEV_NAME "my_dev_name"

static dev_t my_dev;
static int dev_cnt = 1;
static struct cdev *my_cdev;
static char *pbuf;

static int mycdev_open(struct inode *inode, struct file *file)
{
    int major = MAJOR(inode->i_rdev);
    int minor = MINOR(inode->i_rdev);

    printk("%s: major=%d, minor=%d\n", __func__, major, minor);

    return 0;
}

static ssize_t mycdev_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
    printk("%s enter\n", __func__);
    copy_to_user(buf, pbuf, 64);
    return 0;
}

static ssize_t mycdev_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos)
{
    printk("%s enter\n", __func__);
    copy_from_user(pbuf, buf, 64);
    printk("user context:%s\n", pbuf);
    return 0;
}

static const struct file_operations my_cdev_fops = {
    .owner = THIS_MODULE,
    .open = mycdev_open,
    .read = mycdev_read,
    .write = mycdev_write,
};

static int __init mycdev_init(void)
{
    int ret;
    printk("mycdev_init module entry...\n");

    ret = alloc_chrdev_region(&my_dev, 0, dev_cnt, DEV_NAME);
    if (ret)
    {
        printk("alloc chrdev fail.\n");
        return ret;
    }
    my_cdev = cdev_alloc();
    if (!my_cdev)
    {
        printk("cdev alloc fail\n");
        goto unregister_chrdev;
    }
    cdev_init(my_cdev, &my_cdev_fops);
    ret = cdev_add(my_cdev, my_dev, dev_cnt);
    if (ret)
    {
        printk("cdev add fail\n");
        goto cdev_fail;
    }

    pbuf = vmalloc(1024);
    memset(pbuf, 0x0, 1024);
    sprintf(pbuf, "value from kernel ....");

    printk("mycdev add suc...\n");

    printk("successed register char device: %s\n", DEV_NAME);
    printk("Major number = %d,minor number = %d\n", MAJOR(my_dev), MINOR(my_dev));

    return 0;
cdev_fail:
    cdev_del(my_cdev);
unregister_chrdev:
    unregister_chrdev_region(my_dev, dev_cnt);
    return ret;
}

static void __exit mycdev_exit(void)
{
    printk("mycdev_exit ==========================================\n");
    if (my_cdev)
    {
        cdev_del(my_cdev);
    }
    unregister_chrdev_region(my_dev, dev_cnt);
    vfree(pbuf);
}

module_init(mycdev_init);
module_exit(mycdev_exit);

MODULE_LICENSE("GPL");
