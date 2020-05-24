#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/slab.h>

typedef struct irq_data {
    int irq;
    char *p;
    int count;
    struct tasklet_struct test_tasklet;
} test_irq_data;

static int irq;
static char *devname;
test_irq_data *dev_data;

module_param(irq, int, 0644);
module_param(devname, charp, 0644);

static void tasklet_handler(unsigned long data)
{
    test_irq_data *opt_data = (test_irq_data *)data;
    printk("tasklet handler entry: count=%d, taskletstate=%d, let count=%d\n", opt_data->count, opt_data->test_tasklet.state, opt_data->test_tasklet.count);
    return;
}

static irqreturn_t test_handler(int irq, void *dev)
{
    test_irq_data *opt_data = (test_irq_data *)dev;
    printk("irq handler entry: count=%d\n", opt_data->count);
    tasklet_schedule(&opt_data->test_tasklet);
    opt_data->count++;
    return IRQ_HANDLED;
}

static int __init testirq_init(void)
{
    printk("test irq entry...\n");
    int ret;
    dev_data = kzalloc(sizeof(test_irq_data), GFP_KERNEL);
    if (!dev_data) {
        printk("malloc dev_data fail\n");
        return -1;
    }
    ret = request_irq(irq, test_handler, IRQF_SHARED, devname, dev_data);
    if (ret) {
        kfree(dev_data);
        printk("request irq fail\n");
        return -1;
    }
    dev_data->irq = irq;
    dev_data->p = devname;
    dev_data->count = 0;
    tasklet_init(&dev_data->test_tasklet, tasklet_handler, dev_data);
    return 0;
}

static void __exit testirq_exit(void)
{
    free_irq(dev_data->irq, dev_data);
    tasklet_kill(&dev_data->test_tasklet);
    printk("test irq exit...\n");    
}

module_init(testirq_init);
module_exit(testirq_exit);

MODULE_LICENSE("GPL");