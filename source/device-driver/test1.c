#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/sysfs.h>

MODULE_LICENSE("Dual BSD/GPL");

ssize_t abc_show(struct device *dev, struct device_attribute *attr, char *buf);
ssize_t abc_store(struct device *dev, struct device_attribute *attr, 
				const char *buf, size_t count);

struct my_device {
	int test1;
	struct device dev;
};


struct device_attribute my_dev_attr = {
	.attr = {.name = " my_attr", .mode = 644, },
	.show = abc_show, 
	.store = abc_store,
};

int a1 = 0;
ssize_t abc_show(struct device *dev, struct device_attribute *attr, char *buf) 
{
	struct my_device *my_dev1 = container_of(dev, struct my_device, dev);
	printk(KERN_INFO "attr showi : test1=%d\n", my_dev1->test1);
	return scnprintf(buf, PAGE_SIZE, "%d\n", a1);
}

ssize_t abc_store(struct device *dev, struct device_attribute *attr, 
				const char *buf, size_t count) 
{
	sscanf(buf, "%d", &a1);
	return sizeof(int);
}

static struct attribute *my_attrs[] = {
	&my_dev_attr.attr,
	NULL,
};

ATTRIBUTE_GROUPS(my);

static void my_release(struct device *dev)
{
	printk(KERN_INFO "my_release\n");
}

struct device hello_dev = {
	.init_name="hello_dev",
	.release = my_release,
	.groups = my_groups,
};

int hello_init(void)
{
	int ret;

	ret = device_register(&hello_dev);
	printk(KERN_INFO "hello world %d\n", ret);
	return 0;
}

void hello_exit(void)
{
	int ret;

	device_unregister(&hello_dev);
	printk(KERN_INFO "good bye %d\n", ret);
}

module_init(hello_init);
module_exit(hello_exit);
