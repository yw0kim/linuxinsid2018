#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/platform_device.h>
#include <linux/export.h>
#include <linux/property.h>
#include <linux/interrupt.h>

MODULE_LICENSE("Dual BSD/GPL");

int drv_attr = 1;

static ssize_t drv_attr_show(struct device_driver *drv, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%d\n", drv_attr);
}

static ssize_t drv_attr_store(struct device_driver *drv, 
					const char *buf, size_t len)
{
	sscanf(buf, "%d", &drv_attr);
	return sizeof(int);
}

static DRIVER_ATTR_RW(drv_attr);
static struct attribute *my_pdev_driver_attrs[] = {
	&driver_attr_drv_attr.attr,
	NULL
};
ATTRIBUTE_GROUPS(my_pdev_driver);

struct my_drv_data {
	struct platform_device *pdev;
	int irq;
	void __iomem *base;
};

static irqreturn_t my_irq_handler(int, void *);
static int my_pdev_probe(struct platform_device *);

static struct platform_driver my_pdev_driver = {
	.probe      = my_pdev_probe,
	.driver     = {
		.name     = "my_pdev",
		.owner    = THIS_MODULE,
	},
};

static irqreturn_t my_irq_handler(int irq, void *data)
{
	printk(KERN_ALERT "%s Called\n", __FUNCTION__);

	return IRQ_HANDLED;
}

static int my_pdev_probe(struct platform_device *pdev)
{
	printk(KERN_ALERT "%s Called\n", __FUNCTION__);

	int ret = 0;
	struct my_drv_data *m_data;
	struct resource *res;

	m_data = devm_kzalloc(&pdev->dev, sizeof(*m_data), GFP_KERNEL);
	if (!m_data) 
	{
		return -ENOMEM;
	}

	m_data->pdev = pdev;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (unlikely(!res)) {
		pr_err(" Specified Resource Not Available... 1\n");
		return -1;
	}

	printk(KERN_ALERT "\n Memory Area1\n");
	printk(KERN_ALERT "Start:%x,  End:%x Size:%d\n",
		   	(unsigned long)res->start, (unsigned long)res->end, resource_size(res));

	m_data->base = devm_ioremap_resource(&pdev->dev, res);
	printk(KERN_ALERT "iomem resource. base=0x%x\n", m_data->base);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (unlikely(!res)) {
		pr_err(" Specified Resource Not Available... 2\n");
		return -1;
	}

	printk(KERN_ALERT "\n Memory Area2\n");
	printk(KERN_ALERT "Start:%x,  End:%x, size:%d\n", 
			(unsigned long)res->start, (unsigned long)res->end, resource_size(res));

	m_data->base = devm_ioremap_resource(&pdev->dev, res);
	printk(KERN_ALERT "iomem resource. base=0x%x\n", m_data->base);
	
	
	/* irq */
	ret = devm_request_threaded_irq(&pdev->dev, m_data->irq,
			NULL, my_irq_handler, 
			// NULL 자리에 handler가 들어가야함, NULL이 성공적으로 끝나면 
			// my_irq_handler위치가 호출됨 (NULL이면 성공 취급)
			IRQF_SHARED | IRQF_ONESHOT,
			"drv3", m_data);
	
	printk(KERN_ALERT "\n IRQ Number of Device :%d\n", platform_get_irq(pdev, 0));

	dev_info(&pdev->dev, "request_irq() irq=%d, ret=%d\n",
			m_data->irq, ret);

	return 0;  
}

/*
static const struct platform_device_id pdev_id_table[] = {                      
    { "my",    (unsigned long) &foo_id },                   
    { },                                                                        
};                                                                              

static const struct of_device_id pdev_of_match_table[] = { 
    { 
        .compatible = "my,my", 
    }, 
    {  }, 
};
*/


static int pdev_driver_init(void)
{
	int ret;

	ret = platform_driver_register(&my_pdev_driver);
	printk(KERN_INFO "pdev driver register %d\n", ret);
	return 0;
}

static void pdev_driver_exit(void)
{
	platform_driver_unregister(&my_pdev_driver);
}

module_init(pdev_driver_init);
module_exit(pdev_driver_exit);
