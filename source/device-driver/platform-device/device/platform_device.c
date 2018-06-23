#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/platform_device.h>


MODULE_LICENSE("Dual BSD/GPL");

static void my_release(struct device *);

static struct resource my_resources[] = {
	{
		.start          = 0x1000,
		.end            = 0x1FFF,
		.flags          = IORESOURCE_MEM,
	},
	{
		.start          = 0x2000,
		.end            = 0x2FFF,
		.flags          = IORESOURCE_MEM,
	},
	{
		.start          = 6,
		.end            = 6,
		.flags          = IORESOURCE_IRQ,
	}

};  

static struct platform_device my_pdev = {
	.dev = {
		.init_name = "my_dev",
		.release = my_release,
	},
	.name = "my_pdev",
	.id = 0,
	.num_resources  = ARRAY_SIZE(my_resources),
	.resource       = my_resources,
};


static void my_release(struct device *dev)
{
	printk(KERN_INFO "my_release\n");
}

static int pdev_init(void)
{
	int ret;

	ret = platform_device_register(&my_pdev);
	printk(KERN_INFO "platform_device register %d\n", ret);
	return 0;
}

static void pdev_exit(void)
{
	int ret;

	platform_device_unregister(&my_pdev);
	printk(KERN_INFO "platform_device unregister %d\n", ret);
}

module_init(pdev_init);
module_exit(pdev_exit);
