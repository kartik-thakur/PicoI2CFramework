#include <stdlib.h>

#include "error.h"
#include "i2c_dev.h"
#include "pico/stdlib.h"

#include <stdio.h>

int i2c_dev_init(struct i2c_dev *dev)
{
	if (!dev->bus || !dev->addr)
		return -EINVAL;

	gpio_set_function(dev->sda_gpio, GPIO_FUNC_I2C);
	gpio_set_function(dev->scl_gpio, GPIO_FUNC_I2C);

	i2c_init(dev->bus, dev->baudrate);

	return 0;
}

int i2c_dev_read(struct i2c_dev *dev, uint8_t reg, uint8_t *value)
{
	int err = 0;

	err = i2c_write_blocking(dev->bus, dev->addr, &reg, 1, true);
	if (err == PICO_ERROR_GENERIC)
		return err;

	err = i2c_read_blocking(dev->bus, dev->addr, value, 1, false);
	if (err == PICO_ERROR_GENERIC)
		return err;

	return 0;
}


int i2c_dev_write(struct i2c_dev *dev, const uint8_t reg, uint8_t value)
{
	uint8_t i2c_msg[2];
	int err = 0;

	i2c_msg[0] = reg;
	i2c_msg[1] = value;

	err = i2c_write_blocking(dev->bus, dev->addr, i2c_msg, 2, false);
	if (err == PICO_ERROR_GENERIC)
		return err;

	return 0;
}
