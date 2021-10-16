#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "bmp280.h"

int main() {
	struct bmp280 bmp280_dev;
	struct i2c_dev *i2c_dev = &bmp280_dev.dev;
	struct i2c_dev_ops ops;
	uint8_t bmp280_id;
	int ret;

	stdio_init_all();

	sleep_ms(5000);
	printf("BMP280 Sample\n");

	i2c_dev->bus = i2c0;
	i2c_dev->addr = BMP280_ADDRESS;

	ops.init = i2c_dev_init;
	ops.read = i2c_dev_read;
	ops.write = i2c_dev_write;
	i2c_dev->ops = &ops;

	if (i2c_dev->ops->init(i2c_dev)) {
		printf("Unable to initialize I2C device\n");
	} else {
		printf("I2C device initialized\n");
	}

	gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
	gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);

	i2c_dev->ops->write(i2c_dev, BMP280_REG_RESET, 0xB6);

	ret = i2c_dev->ops->read(i2c_dev, BMP280_REG_ID, &bmp280_id);
	if (ret) {
		printf("Unable to probe BMP280 device, err: %d\n", ret);
	} else {
		printf("BMP280 probed successfully, id 0x%x\n",
		       (unsigned int)bmp280_id);
	}

	while (true) {
	}

	return 0;
}
