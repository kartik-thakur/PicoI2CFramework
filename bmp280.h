#ifndef __BMP280_H__
#define __BMP280_H__

#include "i2c_dev.h"

#define BMP280_ADDRESS	0x76
#define BMP280_NUM_REG	11

/*
 * BMP280 Registers to be used.
 */
#define BMP280_REG_ID			0xD0
#define BMP280_REG_RESET		0xE0
#define BMP280_REG_STATUS		0xF3
#define BMP280_REG_CTRL			0xF4
#define BMP280_REG_CONFIG		0xF5
#define BMP280_REG_PRESSURE_MSB		0xF7
#define BMP280_REG_PRESSURE_LSB		0xF8
#define BMP280_REG_PRESSURE_XLSB	0xF9
#define BMP280_REG_TEMPERATURE_MSB	0xFA
#define BMP280_REG_TEMPERATURE_LSB	0xFB
#define BMP280_REG_TEMPERATURE_XLSB	0xFC

#define BMP280_DEVICE_ID		0x58

struct bmp280 {
	struct i2c_dev	dev;
};

#endif
