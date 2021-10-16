#ifndef __I2C_H__
#define __I2C_H__

#include "i2c_bus.h"
#include "pico/stdlib.h"
#include "i2c_bus.h"
#include "i2c_dev.h"

struct i2c_device_list {
	struct i2c_dev		*dev;
	struct i2c_device_list	*next;
	struct i2c_device_list	*prev;
};


inline bool is_reserved_addr(uint8_t addr)
{
	return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

void i2c_framework_init();
void i2c_device_register(struct i2c_dev *dev);
int i2c_device_try_probe(struct i2c_bus *bus, uint8_t device_addr);

#endif
