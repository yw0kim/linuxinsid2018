/*
 * Copyright (C) 2014 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

__u8 READ_BUF = 0x01;

struct foo2_i2c_dev {
	struct device *device;
	struct i2c_adapter adapter;
	struct i2c_msg *msg;

	/* bytes that have been transferred */
	unsigned int tx_bytes;
};

static int foo2_i2c_xfer_single_msg(struct foo2_i2c_dev *foo2_i2c,
					 struct i2c_msg *msg)
{
	int ret, i;
	u8 addr;
	u32 val;
	unsigned int tx_bytes;

	foo2_i2c->msg = msg;

	/*
	 * For a write transaction, load data into the TX FIFO. Only allow
	 * loading up to TX FIFO size - 1 bytes of data since the first byte
	 * has been used up by the slave address
	 */
	tx_bytes = min_t(unsigned int, msg->len, M_TX_RX_FIFO_SIZE - 1);
	if (!(msg->flags & I2C_M_RD)) {
		for (i = 0; i < tx_bytes; i++) {
			val = msg->buf[i];	
			dev_info(foo2_i2c->device, "write val : %u\n", val);
		}
		foo2_i2c->tx_bytes = tx_bytes;
	}



	/*
	 * For a read operation, we now need to load the data from FIFO
	 * into the memory buffer
	 */
	if (msg->flags & I2C_M_RD) {
		for (i = 0; i < msg->len; i++) {
			msg->buf[i] = READ_BUF;
			dev_info(foo2_i2c->device, "read val: %u\n", msg->buf[i]);
		}
	}

	return 0;
}

static int foo2_i2c_xfer(struct i2c_adapter *adapter,
			      struct i2c_msg msgs[], int num)
{
	struct foo2_i2c_dev *foo2_i2c = i2c_get_adapdata(adapter);
	int ret, i;

	/* go through all messages */
	for (i = 0; i < num; i++) {
		ret = foo2_i2c_xfer_single_msg(foo2_i2c, &msgs[i]);
		if (ret) {
			dev_dbg(foo2_i2c->device, "xfer failed\n");
			return ret;
		}
	}

	return num;
}

static uint32_t foo2_i2c_functionality(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

static const struct i2c_algorithm foo2_algo = {
	.master_xfer = foo2_i2c_xfer,
	.functionality = foo2_i2c_functionality,
};

static const struct i2c_adapter_quirks foo2_i2c_quirks = {
	/* need to reserve one byte in the FIFO for the slave address */
	.max_read_len = M_TX_RX_FIFO_SIZE - 1,
};

static int foo2_i2c_probe(struct platform_device *pdev)
{
	int irq, ret = 0;
	struct foo2_i2c_dev *foo2_i2c;
	struct i2c_adapter *adap;
	struct resource *res;

	foo2_i2c = devm_kzalloc(&pdev->dev, sizeof(*foo2_i2c),
				 GFP_KERNEL);
	if (!foo2_i2c)
		return -ENOMEM;

	platform_set_drvdata(pdev, foo2_i2c);
	foo2_i2c->device = &pdev->dev;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	adap = &foo2_i2c->adapter;
	i2c_set_adapdata(adap, foo2_i2c);
	strlcpy(adap->name, "FOO2 I2C adapter", sizeof(adap->name));
	adap->algo = &foo2_algo;
	adap->quirks = &foo2_i2c_quirks;
	adap->dev.parent = &pdev->dev;
	adap->dev.of_node = pdev->dev.of_node;

	return i2c_add_adapter(adap);
}

static int foo2_i2c_remove(struct platform_device *pdev)
{
	struct foo2_i2c_dev *foo2_i2c = platform_get_drvdata(pdev);

	i2c_del_adapter(&foo2_i2c->adapter);

	return 0;
}


static const struct of_device_id foo2_i2c_of_match[] = {
	{ .compatible = "foo2,foo2-i2c" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, foo2_i2c_of_match);

static struct platform_driver foo2_i2c_driver = {
	.driver = {
		.name = "foo2-i2c",
		.of_match_table = foo2_i2c_of_match,
	},
	.probe = foo2_i2c_probe,
	.remove = foo2_i2c_remove,
};
module_platform_driver(foo2_i2c_driver);

MODULE_AUTHOR("Ray Jui <rjui@broadcom.com>");
MODULE_DESCRIPTION("Broadcom iProc I2C Driver");
MODULE_LICENSE("GPL v2");
