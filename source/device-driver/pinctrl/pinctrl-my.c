#include <linux/bitmap.h>
#include <linux/bug.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/gpio/driver.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/irqdesc.h>
#include <linux/init.h>
#include <linux/of_address.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pinctrl/machine.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/platform_device.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");
#define MODULE_NAME "pinctrl-my"

struct my_pinconf {
	const int * confs;
};

struct my_pin {
	unsigned int pin;
	const char *name;
	struct my_pinconf pin_conf;
};

struct my_pin_group {
	const char *name;
	const unsigned int *pins;
	const unsigned int num_pins;
};

struct my_pin_function {
	const char *name;
	const char * const *groups;
	const unsigned int num_groups;
};



struct my_pinctrl {
	struct pinctrl_dev *pctl;
	struct device *dev;
	void __iomem *base;
	int irq;

	const struct my_pin_group *groups;
	unsigned int num_groups;

	const struct my_pin_function *functions;
	unsigned int num_functions;
};

static unsigned int default_confs[] = {0, 1, 2};

static struct my_pin my_pins[] = {
	{
		.pin = 0,
		.name = "pin0",
		.pin_conf = {
			.confs = default_confs,
		},
	},
	{
		.pin = 1,
		.name = "pin2",
		.pin_conf = {
			.confs = default_confs,
		},	
	},
	{
		.pin = 2,
		.name = "pin2",
		.pin_conf = {
			.confs = default_confs,
		},	
	},
	{
		.pin = 3,
		.name = "pin3",
		.pin_conf = {
			.confs = default_confs, 
		},	
	},
	{
		.pin = 4,
		.name = "pin4",
		.pin_conf = {
			.confs = default_confs, 
		},	
	},
	{
		.pin = 5,
		.name = "pin5",
		.pin_conf = {
			.confs = default_confs, 
		},	
	},
	{
		.pin = 6,
		.name = "pin6",
		.pin_conf = {
			.confs = default_confs, 
		},	
	},
	{
		.pin = 7,
		.name = "pin7",
		.pin_conf = {
			.confs = default_confs, 
		},	
	},
};

static const unsigned int gpio0_pins[] = {0, 1, 2, 3};
static const unsigned int nand0_pins[] = {0, 1, 2, 3};
static const unsigned int nand1_pins[] = {4, 5};
static const unsigned int nand2_pins[] = {6, 7};
static const unsigned int uart0_pins[] = {0, 1, 2, 3};
static const unsigned int uart1_pins[] = {4, 5};
static const unsigned int uart2_pins[] = {6, 7};
static const unsigned int i2c0_pins[] = {6, 7};

static const struct my_pin_group my_pin_groups[] = {
	{
		.name = "gpio0",
		.pins = gpio0_pins,
		.num_pins = ARRAY_SIZE(gpio0_pins),
	},
	{
		.name = "nand0",
		.pins = nand0_pins,
		.num_pins = ARRAY_SIZE(nand0_pins),
	},
	{
		.name = "nand1",
		.pins = nand1_pins,
		.num_pins = ARRAY_SIZE(nand1_pins),
	},
	{
		.name = "nand2",
		.pins = nand2_pins,
		.num_pins = ARRAY_SIZE(nand2_pins),
	},
	{
		.name = "uart0",
		.pins = uart0_pins,
		.num_pins = ARRAY_SIZE(uart0_pins),
	},
	{
		.name = "uart1",
		.pins = uart1_pins,
		.num_pins = ARRAY_SIZE(uart1_pins),
	},
	{
		.name = "uart2",
		.pins = uart2_pins,
		.num_pins = ARRAY_SIZE(uart2_pins),
	},
	{
		.name = "i2c0",
		.pins = i2c0_pins,
		.num_pins = ARRAY_SIZE(i2c0_pins),
	},
};



// pinctrl_ops
static int my_get_groups_count(struct pinctrl_dev *pctldev)
{
	return ARRAY_SIZE(my_pin_groups);
}

static const char *my_get_group_name(struct pinctrl_dev *pctldev,
		unsigned selector)
{
	return my_pin_groups[selector].name;
}

static int my_get_group_pins(struct pinctrl_dev *pctldev, unsigned selector,
		const unsigned **pins,
		unsigned *num_pins)
{
	*pins = (unsigned *) my_pin_groups[selector].pins;
	*num_pins = my_pin_groups[selector].num_pins;
	return 0;
}

static struct pinctrl_ops my_pctrl_ops = {
	.get_groups_count = my_get_groups_count,
	.get_group_name = my_get_group_name,
	.get_group_pins = my_get_group_pins,
};
////


// pinmux_ops
static const char * const gpio_groups[] = { "gpio0" };
static const char * const nand_groups[] = { "nand0", "nand1", "nand2" };
static const char * const uart_groups[] = { "uart0", "uart1", "uart2" };
static const char * const i2c_groups[] = { "i2c" };

static const struct my_pin_function my_pin_functions[] = {
	{
		.name = "gpio",
		.groups = gpio_groups,
		.num_groups = ARRAY_SIZE(gpio_groups),
	},
	{
		.name = "nand",
		.groups = nand_groups,
		.num_groups = ARRAY_SIZE(nand_groups),
	},
	{
		.name = "uart",
		.groups = uart_groups,
		.num_groups = ARRAY_SIZE(uart_groups),
	},
	{
		.name = "i2c",
		.groups = i2c_groups,
		.num_groups = ARRAY_SIZE(i2c_groups),
	},
};

static int my_get_functions_count(struct pinctrl_dev *pctldev)
{
	return ARRAY_SIZE(my_pin_functions);
}

static const char *my_get_fname(struct pinctrl_dev *pctldev, unsigned selector)
{
	return my_pin_functions[selector].name;
}

static int my_get_groups(struct pinctrl_dev *pctldev, unsigned selector,
		const char * const **groups,
		unsigned * const num_groups)
{
	*groups = my_pin_functions[selector].groups;
	*num_groups = my_pin_functions[selector].num_groups;
	return 0;
}

/*
static int my_set_mux(struct pinctrl_dev *pctldev, unsigned selector,
		unsigned group)
{
	u8 regbit = (1 << selector + group);

	writeb((readb(MUX)|regbit), MUX)
		return 0;
}
*/

static struct pinmux_ops my_pinmux_ops = {
	.get_functions_count = my_get_functions_count,
	.get_function_name = my_get_fname,
	.get_function_groups = my_get_groups,
//	.set_mux = my_set_mux,
	.strict = true,
};
////


// pinconf ops

static int my_pin_config_get(struct pinctrl_dev *pctldev,
		unsigned offset,
		unsigned long *config)
{
	printk(KERN_INFO "my_pin_config_get called\n");
}

static int my_pin_config_set(struct pinctrl_dev *pctldev,
		unsigned offset,
		unsigned long *config,
		unsigned int num_configs)
{
	printk(KERN_INFO "my_pin_config_set called\n");
}

static int my_pin_config_group_get (struct pinctrl_dev *pctldev,
		unsigned selector,
		unsigned long *config)
{
	printk(KERN_INFO "my_pin_config_group_get called\n");
}

static int my_pin_config_group_set (struct pinctrl_dev *pctldev,
		unsigned selector,
		unsigned long *config,
		unsigned int num_configs)
{
	printk(KERN_INFO "my_pin_config_group_set called\n");
}

static struct pinconf_ops my_pinconf_ops = {
	.pin_config_get = my_pin_config_get,
	.pin_config_set = my_pin_config_set,
	.pin_config_group_get = my_pin_config_group_get,
	.pin_config_group_set = my_pin_config_group_set,
};

//// 


static struct pinctrl_desc my_pinctrl_desc = {
	.name = "my-pinctrl",
	.pctlops = &my_pctrl_ops,
	.pmxops = &my_pinmux_ops,
	.confops = &my_pinconf_ops,
};

static int my_pinctrl_probe(struct platform_device *pdev) 
{
	struct my_pinctrl *pinctrl;
	struct resource *res;
	int i;
	struct pinctrl_pin_desc *pins;
	unsigned int num_pins = ARRAY_SIZE(my_pins);

	pinctrl = devm_kzalloc(&pdev->dev, sizeof(*pinctrl), GFP_KERNEL);
	if (!pinctrl)
		return -ENOMEM;

	pinctrl->dev = &pdev->dev;
//	platfrom_set_drvdata(pdev, pinctrl);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	pinctrl->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(pinctrl->base))
		return PTR_ERR(pinctrl->base);

	pins = devm_kcalloc(&pdev->dev, num_pins, sizeof(*pins), GFP_KERNEL);
	if (!pins)
		return -ENOMEM;

	for (i = 0; i < num_pins; i++) {
		pins[i].number = my_pins[i].pin;
		pins[i].name = my_pins[i].name;
	}

	pinctrl->groups = my_pin_groups;
	pinctrl->num_groups = ARRAY_SIZE(my_pin_groups);
	pinctrl->functions = my_pin_functions;
	pinctrl->num_functions = ARRAY_SIZE(my_pin_functions);
	my_pinctrl_desc.pins = pins;
	my_pinctrl_desc.npins = num_pins;

	pinctrl->pctl = pinctrl_register(&my_pinctrl_desc, &pdev->dev,
			pinctrl);
	if (IS_ERR(pinctrl->pctl)) {
		dev_err(&pdev->dev, "pinctrl register FAIL\n");
		return PTR_ERR(pinctrl->pctl);
	}

	return 0;
}

static const struct of_device_id my_pinctrl_match[] = {
	{ .compatible = "my,my-pinctrl" },
	{}
};



static struct platform_driver my_pinctrl_driver = {
	.probe = my_pinctrl_probe,
	.driver = {
		.name = "my-pinctrl",
		.of_match_table = my_pinctrl_match,
	},
};

static int __init my_pinctrl_init(void) 
{
	int ret;
	ret = platform_driver_register(&my_pinctrl_driver);
	printk(KERN_INFO "platform_driver_register ret : %d\n", ret);

	return 0;
}

static void my_pinctrl_exit(void)
{
	platform_driver_unregister(&my_pinctrl_driver);
}

module_init(my_pinctrl_init);
module_exit(my_pinctrl_exit);
