#include <stdlib.h>
#include <stdio.h>

#include "i2c_core.h"
#include "i2c_bus.h"

struct i2c_device_list *i2c_devices = NULL;

void i2c_framework_init()
{
	i2c_bus_init();
}

void i2c_device_register(struct i2c_dev *dev)
{
	if (!i2c_devices) {
		i2c_devices = calloc(1, sizeof(struct i2c_device_list));
		if (!i2c_devices) {
			printf("Unable to register I2C device, "
			       "no memory left on the device\n");
			return;
		}

		i2c_devices->dev = dev;
	} else {
		struct i2c_device_list	*device;

		for (device = i2c_devices; device != NULL;
		     device = device->next) {
			if (!device->next) {
				device->next = calloc(1,
					sizeof(struct i2c_device_list));
				if (!device->next) {
					printf("Unable to register I2C device, "
					      "no memory left on the device\n");
					return;
				}

				device->next->dev = dev;
				device->next->prev = device;
				break;
			}
		}
	}	
}

void i2c_device_try_probe(struct i2c_bus *bus, uint8_t device_addr)
{

}
