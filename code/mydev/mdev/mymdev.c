#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>

#define DEV_NAME "my_dev"
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

static const struct file_operations my_misc_fops = {
    .owner = THIS_MODULE,
    .open = mycdev_open,
    .read = mycdev_read,
    .write = mycdev_write,
};

static struct miscdevice my_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEV_NAME,
    .fops = &my_misc_fops,
};

static int __init mymdev_init(void)
{
    int ret;
    printk("mymdev_init module entry...\n");

    ret = misc_register(&my_misc_device);
    if (ret)
    {
        printk("register my misc device fail.\n");
        return ret;
    }

    pbuf = vmalloc(1024);
    memset(pbuf, 0x0, 1024);
    sprintf(pbuf, "value from kernel ....");

    printk("mycdev add suc...\n");

    printk("successed register misc device: %s\n", DEV_NAME);
    printk("Major number = %d,minor number = %d\n", MAJOR(my_misc_device.this_device->devt), MINOR(my_misc_device.minor));

    return 0;
}

static void __exit mymdev_exit(void)
{
    printk("mymdev_exit ==========================================\n");
    misc_deregister(&my_misc_device);
    vfree(pbuf);
}

module_init(mymdev_init);
module_exit(mymdev_exit);

MODULE_LICENSE("GPL");
