/*
 * GPIO driver for LPC32xx SoC
 *
 * Author: Kevin Wells <kevin.wells@nxp.com>
 *
 * Copyright (C) 2010 NXP Semiconductors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/module.h>

#include <mach/hardware.h>
#include <mach/platform.h>

#define	INPUT					0
#define OUTPUT					1

#define GPIO01_PIN_TO_BIT(x) 	(1 << (x)) // x : pin 
#define GPIO01_PIN_IN_SEL(x, y) (((y) << (x)) & 0xffff) // x : pin, y : reg

#define FOO_GPIO_P0_MAX 4
#define FOO_GPIO_P1_MAX 2

#define FOO_GPIO_P0_GRP 0
#define FOO_GPIO_P1_GRP (FOO_GPIO_P0_GRP + FOO_GPIO_P0_MAX)

/*
struct gpio_regs {
	void __iomem *inp_state;
	void __iomem *outp_state;
	void __iomem *outp_set;
	void __iomem *outp_clr;
	void __iomem *dir_set;
	void __iomem *dir_clr;
};
*/
/*
 * GPIO names
 */
struct gpio_regs {
	unsigned int dat;
	unsigned int set;
	unsigned int clr;
	unsigned int dirout;
	unsigned int dirin;
};


static const char *gpio_p0_names[FOO_GPIO_P0_MAX] = {
	"p0.0", "p0.1", "p0.2", "p0.3",
};

static const char *gpio_p1_names[FOO_GPIO_P1_MAX] = {
	"p1.0", "p1.1",
};

static struct gpio_regs gpio_grp_regs_p0 = {
	.dat = 0,
	.set = 0,
	.clr = 0,
	.dirout = 0x0000,
	.dirin = 0x000f,
};

static struct gpio_regs gpio_grp_regs_p1 = {
	.dat = 0,
	.set = 0,
	.clr = 0,
	.dirout = 0x0000,
	.dirin = 0x000f,
};

struct foo_gpio_chip {
	struct gpio_chip chip;
	struct gpio_regs *gpio_grp;
};

static void __set_gpio_dir_p01(struct foo_gpio_chip *group,
		unsigned pin, int input, int way) 
{
	struct gpio_regs *regs = group->gpio_grp;

	if(way == INPUT) {
		regs->dirin |= GPIO01_PIN_TO_BIT(pin);
		u32 dirout = GPIO01_PIN_IN_SEL(pin, regs->dirout);
		regs->dirout &= ~dirout;
	}
	else if(way == OUTPUT) {
		regs->dirout |= GPIO01_PIN_TO_BIT(pin);
		u32 dirin = GPIO01_PIN_IN_SEL(pin, regs->dirin);
	   	regs->dirin &= ~dirin;	
	}
}

static int foo_gpio_dir_input_p01(struct gpio_chip *chip,
		unsigned pin) 
{
	struct foo_gpio_chip *group = gpiochip_get_data(chip);

	__set_gpio_dir_p01(group, pin, 1, INPUT);

	return 0;
}

static int foo_gpio_dir_output_p01(struct gpio_chip *chip,
		unsigned pin) 
{
	struct foo_gpio_chip *group = gpiochip_get_data(chip);

	__set_gpio_dir_p01(group, pin, 1, OUTPUT);

	return 0;
}

static int __foo_get_gpio_dat_p01(struct foo_gpio_chip *group,
		unsigned pin)
{
	u32 dat = GPIO01_PIN_IN_SEL(pin, group->gpio_grp->dat);

	return dat >> pin;
}

static int foo_gpio_get_value_p01(struct gpio_chip *chip, unsigned pin)
{
	struct foo_gpio_chip *group = gpiochip_get_data(chip);

	return __foo_get_gpio_dat_p01(group, pin);
}

static void __foo_set_gpio_dat_p01(struct foo_gpio_chip *group,
		unsigned pin)
{
	struct gpio_regs *regs = group->gpio_grp;

	regs->dat |= GPIO01_PIN_TO_BIT(pin);
}

static int foo_gpio_set_value_p01(struct gpio_chip *chip, unsigned pin,
		int value)
{
	struct foo_gpio_chip *group = gpiochip_get_data(chip);

	__foo_set_gpio_dat_p01(group, pin, value);
}

static int foo_gpio_request(struct gpio_chip *chip, unsigned pin)
{
	if (pin < chip->ngpio)
		return 0;

	return -EINVAL;
}

static int foo_gpio_to_irq_p01(struct gpio_chip *chip, unsigned offset)
{
	return -ENXIO;
}

static struct foo_gpio_chip foo_gpiochip[] = {
	{
		.chip = {
			.label				= "gpio_p0",
			.direction_input	= foo_gpio_dir_input_p01,
			.get 				= foo_gpio_get_value_p01,
			.direction_output 	= foo_gpio_dir_output_p01,
			.set				= foo_gpio_set_value_p01,
			.request			= foo_gpio_request,
			.to_irq				= foo_gpio_to_irq_p01,
			.base				= FOO_GPIO_P0_GRP,
			.ngpio				= FOO_GPIO_P0_MAX,
			.names				= gpio_p0_names,
			.can_sleep			= false,
		},
		.gpio_grp = &gpio_grp_regs_p0,
	},
	{
		.chip = {
			.label				= "gpio_p1",
			.direction_input	= foo_gpio_dir_input_p01,
			.get 				= foo_gpio_get_value_p01,
			.direction_output 	= foo_gpio_dir_output_p01,
			.set				= foo_gpio_set_value_p01,
			.request			= foo_gpio_request,
			.to_irq				= foo_gpio_to_irq_p01,
			.base				= FOO_GPIO_P1_GRP,
			.ngpio				= FOO_GPIO_P1_MAX,
			.names				= gpio_p1_names,
			.can_sleep			= false,
		},
		.gpio_grp = &gpio_grp_regs_p1,
	},
};

static int foo_gpio_probe(struct platform_device *pdev)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(foo_gpiochip); i++) {
		if(pdev->dev.of_node) {
//			foo_gpiochip[i].chip.of
//			foo_gpiochip[i].chip.of_gpio_n_cells = 2;
			foo_gpiochip[i].chip.of_node = pdev->dev.of_node;
		}
		devm_gpiochip_add_data(&pdev->dev, &foo_gpiochip[i].chip,
				&foo_gpiochip[i]);
	}

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id foo_gpio_of_match[] = {
	{ .compatible = "foo, foo-gpio", },
	{ },
};
#endif

static struct platform_driver foo_gpio_driver = {
	.driver 	= {
		.name = "foo-gpio",
		.of_match_table = of_match_ptr(foo_gpio_of_match),
	},
	.probe 		= foo_gpio_probe,
};

module_platform_driver(foo_gpio_driver);


