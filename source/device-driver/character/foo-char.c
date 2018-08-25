#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
 
static char *buffer = NULL;
dev_t dev_id;
struct cdev cdev;
struct class *class;
struct device *dev;
 
int foo_device_open(struct inode *inode, struct file *filp) {
    printk(KERN_ALERT "foo_device open function called\n");
    return 0;
}
 
int foo_device_release(struct inode *inode, struct file *filp) {
    printk(KERN_ALERT "foo device release function called\n");
    return 0;
}
 
ssize_t foo_device_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
    printk(KERN_ALERT "foo_device write function called\n");
	// strcpy(buffer, bufr;
	copy_from_user(buf, buffer, 1024);
    return count;
}
 
ssize_t foo_device_read(struct file *filp, char *buf, size_t count, loff_t *f_pos) {
    printk(KERN_ALERT "foo_device read function called\n");
    copy_to_user(buf, buffer, 1024);
    return count;
}

long foo_device_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk(KERN_ALERT, "foo device ioctl function caleed\n");
	return 0;
}
 
static struct file_operations foo_fops = {
    .read = foo_device_read,
    .write = foo_device_write,
    .open = foo_device_open,
	.unlocked_ioctl = foo_device_ioctl,
    .release = foo_device_release
};
 
int __init foo_device_init(void) {
	int ret = 0;

	ret = alloc_chrdev_region(&dev_id, 0, 1, "foo_cdev");
	if(ret) {
		printk("alloc_chrdev_region err %d\n", ret);
		goto F_ALLOC;
	}

	cdev_init(&cdev, &foo_fops);
	cdev.owner = THIS_MODULE;

	ret = cdev_add(&cdev, dev_id, 1);
	if(ret) {
		printk("cdev_add err %d\n", ret);
		goto F_ADD;
	}

	class = class_create(THIS_MODULE, "foo_cdev");
	if(IS_ERR(class)){
		ret = PTR_ERR(class);
		printk("class_create err %d\n", ret);
		goto F_CLASS;
	}
	
	dev = device_create(class, NULL, dev_id, NULL, "foo_cdev");
	if(IS_ERR(dev)) {
		ret = PTR_ERR(dev);
		printk("device_create err %d\n", ret);
		goto F_DEV;
	}

    buffer = (char*)kmalloc(1024, GFP_KERNEL);
    if(buffer != NULL) 
        memset(buffer, 0, 1024);

F_DEV:
	class_destroy(class);
F_CLASS:
	cdev_del(&cdev);
F_ADD:
	unregister_chrdev_region(dev_id, 1);
F_ALLOC:
    return ret;
}
 
void __exit foo_device_exit(void) {
	device_destroy(class, dev_id);
	class_destroy(class);
	cdev_del(&cdev);
	unregister_chrdev_region(dev_id, 1);

    printk(KERN_ALERT "driver cleanup successful\n");
    kfree(buffer);
}
 
module_init(foo_device_init);
module_exit(foo_device_exit);
MODULE_LICENSE("GPL");
