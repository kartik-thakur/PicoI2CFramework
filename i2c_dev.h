#ifndef __I2C_DEV_H__
#define __I2C_DEV_H__

#include "hardware/i2c.h"

struct i2c_dev;

struct i2c_dev_ops {
	int (*init)(struct i2c_dev *dev);
	int (*read)(struct i2c_dev *dev, uint8_t reg, uint8_t *value);
	int (*write)(struct i2c_dev *dev, uint8_t reg, uint8_t value);
};

struct i2c_dev_reg_val {
	uint8_t		reg;
	uint8_t		val;
};

struct i2c_dev {
	i2c_inst_t		*bus;
	uint8_t			addr;
	uint			baudrate;
	uint			sda_gpio;
	uint			scl_gpio;


	struct i2c_dev_ops	*ops;
};

int i2c_dev_register_ops(struct i2c_dev *dev, struct i2c_dev_ops *ops);
int i2c_dev_init(struct i2c_dev *dev);
int i2c_dev_read(struct i2c_dev *dev, uint8_t reg, uint8_t *value);
int i2c_dev_write(struct i2c_dev *dev, uint8_t reg, uint8_t value);

#endif
