#ifndef __I2C_BUS_H__
#define __I2C_BUS_H__

#include <hardware/i2c.h>

#define I2C_BUS_MAX_DEVICES	0x80

struct i2c_bus {
	i2c_inst_t	*bus;
	uint		baudrate;

	uint		sda_gpio;
	uint		scl_gpio;
};

void i2c_bus_init();

#endif
