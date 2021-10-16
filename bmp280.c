#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "bmp280.h"
#include "i2c_dev.h"
#include "i2c_core.h"


static int bmp280_init(struct i2c_dev *dev)
{
	uint8_t bmp280_id;
	int err;

	err =  dev->ops->read(dev, BMP280_REG_ID, &bmp280_id);
	if (err) {
		printf("Unable to probe BMP280 device, err: %d\n", err);
	} else {
		printf("BMP280 probed successfully, id 0x%x\n",
		       (unsigned int)bmp280_id);
	}

	return 0;
}

static struct i2c_dev_ops bmp280_ops = {
	.init	= bmp280_init,
	.read	= i2c_dev_read,
	.write	= i2c_dev_write,
};

int bmp280_module_init()
{
	struct i2c_dev *dev;

	dev = calloc(1, sizeof(struct i2c_dev));
	if (!dev)
		return -ENOMEM;

	dev->bus	= i2c0;
	dev->addr	= BMP280_ADDRESS;
	dev->ops	= &bmp280_ops;

	i2c_device_register(dev);

	return 0;
}

int main() {
	stdio_init_all();

	sleep_ms(5000);
	printf("Testing I2c Framework\n");

	bmp280_module_init();
	printf("BMP280 driver registered\n");

	printf("Initializing I2C framework\n");
	i2c_framework_init();
	printf("Done\n");

	while (true) {
	}

	return 0;
}
