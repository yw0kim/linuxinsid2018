#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>

MODULE_LICENSE("Dual BSD/GPL");

struct my_drv_data {
	struct platform_device *pdev;
	int irq;
	void iomem *base;
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

static irqretunr_t my_irq_handler(int irq, void *data)
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

	m_data = devm_kzalloc(&pdev->dev, sizeof(struct my_drv_data), GFP_KERNEL);
	if (!m_data) 
	{
		return -ENOMEM;
	}

	m_data->dev = pdev;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (unlikely(res1)) {
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
	int ret;

	platform_driver_unregister(&my_pdev_driver);
	printk(KERN_INFO "pdev driver unregister %d\n", ret);
}

module_init(pdev_driver_init);
module_exit(pdev_driver_exit);
