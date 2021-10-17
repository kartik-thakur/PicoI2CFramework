#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "bmp280.h"
#include "i2c_dev.h"
#include "i2c_core.h"

static int bmp280_update_status(struct i2c_dev *dev)
{
	struct bmp280 *sensor;
	int err = 0;

	sensor = (struct bmp280*)dev->priv_data;
	err = i2c_dev_read(dev, BMP280_REG_STATUS, &sensor->status);
	if (err)
		printf("Failed to read device status\n");

	return err;
}

static int bmp280_reset(struct i2c_dev *dev)
{
	i2c_dev_write(dev, BMP280_REG_RESET, BMP280_RESET_VALUE);
	sleep_ms(2);

	return 0;
};

struct bmp280_device_configuration device_configurations[] = {
	{
		.power_mode			= NORMAL_MODE,
		.temperature_oversampling	= ULTRA_HIGH_RESOLUTION,
		.pressure_oversampling		= ULTRA_HIGH_RESOLUTION,
		.iir_filter			= FILTER_16,
		.standby_time			= STANDBY_TIME_0_5_MS,
	}, {
		.power_mode			= NORMAL_MODE,
		.temperature_oversampling	= HIGH_RESOLUTION,
		.pressure_oversampling		= HIGH_RESOLUTION,
		.iir_filter			= FILTER_8,
		.standby_time			= STANDBY_TIME_62_5_MS,
	},
};

static int bmp280_get_raw_data(struct i2c_dev *dev)
{
	struct bmp280 *sensor;
	uint8_t raw_data[6] = { 0 };
	int err = 0;

	sensor = (struct bmp280*)dev->priv_data;

	err = i2c_dev_burst_read(dev, BMP280_REG_PRESSURE_MSB, raw_data, 6);
	if (err)
		return err;

	sensor->raw_pressure = (int32_t) ((((uint32_t) (raw_data[0])) << 12)
					| (((uint32_t) (raw_data[1])) << 4)
					| ((uint32_t) raw_data[2] >> 4));

	sensor->raw_temperature = (int32_t) ((((uint32_t) (raw_data[3])) << 12)
					| (((uint32_t) (raw_data[4])) << 4)
					| ((uint32_t) raw_data[5] >> 4));

	return 0;
}

static int bmp280_get_calib_param(struct i2c_dev *dev)
{
	struct bmp280 *sensor;
	uint8_t calib_data[BMP280_NUM_CALIB_PARAM] = { 0 };
	int err = 0;

	sensor = (struct bmp280*) dev->priv_data;
	err = i2c_dev_burst_read(dev, BMP280_REG_CALIB_00, calib_data,
			         BMP280_NUM_CALIB_PARAM);
	if (err)
		return err;

	sensor->calib_param.dig_t1 =
		(uint16_t) ((((uint16_t) calib_data[T1_MSB]) << 8) |
		((uint16_t) calib_data[T1_LSB]));

	sensor->calib_param.dig_t2 =
		(int16_t) (((int16_t) calib_data[T2_MSB] << 8) |
		((int16_t) calib_data[T2_LSB]));

	sensor->calib_param.dig_t3 =
		(int16_t) (((int16_t) calib_data[T3_MSB] << 8) |
		((int16_t) calib_data[T3_LSB]));

	sensor->calib_param.dig_p1 =
		(uint16_t) (((uint16_t) calib_data[P1_MSB] << 8) |
		((uint16_t) calib_data[P1_LSB]));

	sensor->calib_param.dig_p2 =
		(int16_t) (((int16_t) calib_data[P2_MSB] << 8) |
		((int16_t) calib_data[P2_LSB]));

	sensor->calib_param.dig_p3 =
		(int16_t) (((int16_t) calib_data[P3_MSB] << 8) |
		((int16_t) calib_data[P3_LSB]));

	sensor->calib_param.dig_p4 =
		(int16_t) (((int16_t) calib_data[P4_MSB] << 8) |
		((int16_t) calib_data[P4_LSB]));

	sensor->calib_param.dig_p5 =
		(int16_t) (((int16_t) calib_data[P5_MSB] << 8) |
		((int16_t) calib_data[P5_LSB]));

	sensor->calib_param.dig_p6 =
		(int16_t) (((int16_t) calib_data[P6_MSB] << 8) |
		((int16_t) calib_data[P6_LSB]));

	sensor->calib_param.dig_p7 =
		(int16_t) (((int16_t) calib_data[P7_MSB] << 8) |
		((int16_t) calib_data[P7_LSB]));

	sensor->calib_param.dig_p8 =
		(int16_t) (((int16_t) calib_data[P8_MSB] << 8) |
		((int16_t) calib_data[P8_LSB]));

	sensor->calib_param.dig_p9 =
		(int16_t) (((int16_t) calib_data[P9_MSB] << 8) |
		((int16_t) calib_data[P9_LSB]));

	return 0;
}

static int bmp280_get_temperature(struct i2c_dev *dev)
{
	struct bmp280 *sensor;
	int32_t temperature[2];
	int err = 0;

	sensor = (struct bmp280*)dev->priv_data;

	err = bmp280_get_raw_data(dev);
	if (err)
		return err;

	err = bmp280_get_calib_param(dev);
	if (err)
		return err;

	temperature[0] = ((((sensor->raw_temperature / 8) -
				((int32_t) sensor->calib_param.dig_t1 << 1))) *
				((int32_t) sensor->calib_param.dig_t2)) / 2048;
	temperature[1] = (((((sensor->raw_temperature / 16) -
			 ((int32_t) sensor->calib_param.dig_t1)) *
			 ((sensor->raw_temperature / 16) -
			 ((int32_t) sensor->calib_param.dig_t1))) / 4096) *
			 ((int32_t) sensor->calib_param.dig_t3)) / 16384;

	sensor->calib_param.t_fine = temperature[0] + temperature[1];
	sensor->temperature = (sensor->calib_param.t_fine * 5 + 128) / 256;

	return 0;
}

static int bmp280_get_pressure(struct i2c_dev *dev)
{
	struct bmp280 *sensor;
	int32_t pressure[2];
	int err = 0;

	sensor = (struct bmp280*)dev->priv_data;

	err = bmp280_get_raw_data(dev);
	if (err)
		return err;

	err = bmp280_get_calib_param(dev);
	if (err)
		return err;

	pressure[0] = (((int32_t) sensor->calib_param.t_fine) / 2) -
			(int32_t) 64000;
	pressure[1] = (((pressure[0] / 4) * (pressure[0] / 4)) / 2048) *
			((int32_t) sensor->calib_param.dig_p6);
	pressure[1] = pressure[1] + ((pressure[0] *
			((int32_t) sensor->calib_param.dig_p5)) * 2);
	pressure[1] = (pressure[1] / 4) +
			(((int32_t) sensor->calib_param.dig_p4) * 65536);
	pressure[0] = (((sensor->calib_param.dig_p3 *
			(((pressure[0] / 4) * (pressure[0] / 4)) / 8192)) / 8) +
			((((int32_t) sensor->calib_param.dig_p2) *
			pressure[0]) / 2)) / 262144;
	pressure[0] = ((((32768 + pressure[0])) *
			((int32_t) sensor->calib_param.dig_p1)) / 32768);
	sensor->pressure = (uint32_t)(((int32_t)(1048576 - sensor->raw_pressure)
			- (pressure[1] / 4096)) * 3125);

	if (pressure[0]) {
		if (sensor->pressure < 0x80000000) {
			sensor->pressure = (sensor->pressure << 1) /
				((uint32_t) pressure[0]);
		} else {
			sensor->pressure = (sensor->pressure /
				(uint32_t) pressure[0]) * 2;
		}

		pressure[0] = (((int32_t) sensor->calib_param.dig_p9) *
				((int32_t) (((sensor->pressure / 8) *
				(sensor->pressure / 8)) / 8192))) / 4096;
		pressure[1] = (((int32_t) (sensor->pressure / 4)) *
				((int32_t) sensor->calib_param.dig_p8)) / 8192;
		sensor->pressure = (uint32_t) ((int32_t) sensor->pressure +
				((pressure[0] + pressure[1] +
				sensor->calib_param.dig_p7) / 16));
	} else {
		sensor->pressure = 0;
	}

	return 0;
}

static int bmp280_configure(struct i2c_dev *dev,
		struct bmp280_device_configuration *device_configuration)
{
	struct bmp280 *sensor;
	uint8_t control = 0;
	uint8_t config = 0;
	int err = 0;

	sensor = (struct bmp280*)dev->priv_data;
	bmp280_reset(dev);

	/* Read control register to determine current settings. */
	err = i2c_dev_read(dev, BMP280_REG_CTRL, &sensor->control);
	if (err)
		return err;

	control |= (device_configuration->pressure_oversampling << 2);
	control |= (device_configuration->temperature_oversampling << 5);

	if (control != sensor->control) {
		sensor->control = control;
		err = i2c_dev_write(dev, BMP280_REG_CTRL, control);
		if (err)
			return err;
	}

	/* Read config regiter. */
	err = i2c_dev_read(dev, BMP280_REG_CONFIG, &sensor->config);
	if (err)
		return err;

	config |= (device_configuration->iir_filter << 2);
	config |= (device_configuration->standby_time << 5);

	if (config != sensor->config) {
		sensor->config = config;
		err = i2c_dev_write(dev, BMP280_REG_CONFIG, config);
		if (err)
			return err;
	}

	control |= device_configuration->power_mode;
	if (control != sensor->control) {
		sensor->control = control;
		err = i2c_dev_write(dev, BMP280_REG_CTRL, control);
		if (err)
			return err;
	}

	return err;
}

static int bmp280_init(struct i2c_dev *dev)
{
	struct bmp280 *sensor;
	int err;

	sensor = calloc(1, sizeof(struct bmp280));
	if (!sensor)
		return -ENOMEM;

	err =  i2c_dev_read(dev, BMP280_REG_ID, &sensor->id);
	if (err) {
		printf("Unable to probe BMP280 device, err: %d\n", err);
		goto out;
	}

	dev->priv_data = (void*)sensor;

	bmp280_reset(dev);
	bmp280_update_status(dev);
	bmp280_configure(dev, &device_configurations[0]);

	while(true) {
		bmp280_get_temperature(dev);
		bmp280_get_pressure(dev);
		printf("Temperature: %d\n", sensor->temperature);
		printf("Pressure   : %d\n\n\n", sensor->pressure);
		sleep_ms(1000);
	}

out:
	if (err) {
		free(sensor);
	} else {
		printf("BMP280 probed successfully, id 0x%x\n",
		       (unsigned int)sensor->id);
	}

	return err;
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

	return 0;
}
