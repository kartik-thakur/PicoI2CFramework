#ifndef __BMP280_H__
#define __BMP280_H__

#include "i2c_dev.h"

#define BMP280_ADDRESS	0x76

/*
 * BMP280 Registers to be used.
 */
#define BMP280_REG_CALIB_00		0x88
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
#define BMP280_RESET_VALUE		0xB6
#define BMP280_STATUS_MEASURING		0x03
#define BMP280_NUM_CALIB_PARAM		UINT8_C(24)

enum bmp280_power_mode {
	SLEEP_MODE	= 0x0,
	FORCED_MODE	= 0x1,
	NORMAL_MODE	= 0x3,
};

enum bmp280_oversampling {
	NONE,
	ULTRA_LOW_POWER,
	LOW_POWER,
	STANDARD_RESOLUTION,
	HIGH_RESOLUTION,
	ULTRA_HIGH_RESOLUTION,
};

enum bmp280_iir_filter {
	FILTER_OFF,
	FILTER_2,
	FILTER_4,
	FILTER_8,
	FILTER_16,
};

enum bmp280_standby_time {
	STANDBY_TIME_0_5_MS	= 0x0,
	STANDBY_TIME_62_5_MS	= 0x1,
	STANDBY_TIME_125_MS	= 0x2,
	STANDBY_TIME_250_MS	= 0x3,
	STANDBY_TIME_500_MS	= 0x4,
	STANDBY_TIME_1000_MS	= 0x5,
	STANDBY_TIME_2000_MS	= 0x6,
	STANDBY_TIME_4000_MS	= 0x7,
};

struct bmp280_device_configuration {
	enum bmp280_power_mode		power_mode;
	enum bmp280_oversampling	temperature_oversampling;
	enum bmp280_oversampling	pressure_oversampling;
	enum bmp280_iir_filter		iir_filter;

	/* To be used during normal power mode */
	enum bmp280_standby_time	standby_time;
};

enum bmp280_calib_param_pos {
	T1_LSB,
	T1_MSB,
	T2_LSB,
	T2_MSB,
	T3_LSB,
	T3_MSB,
	P1_LSB,
	P1_MSB,
	P2_LSB,
	P2_MSB,
	P3_LSB,
	P3_MSB,
	P4_LSB,
	P4_MSB,
	P5_LSB,
	P5_MSB,
	P6_LSB,
	P6_MSB,
	P7_LSB,
	P7_MSB,
	P8_LSB,
	P8_MSB,
	P9_LSB,
	P9_MSB,
};

struct bmp280_calib_param {
	uint16_t	dig_t1;
	int16_t		dig_t2;
	int16_t		dig_t3;
	uint16_t	dig_p1;
	int16_t		dig_p2;
	int16_t		dig_p3;
	int16_t		dig_p4;
	int16_t		dig_p5;
	int16_t		dig_p6;
	int16_t		dig_p7;
	int16_t		dig_p8;
	int16_t		dig_p9;
	int32_t		t_fine;
};

struct bmp280 {
	struct i2c_dev				dev;

	struct bmp280_device_configuration	device_config;

	uint8_t					id;
	uint8_t					status;
	uint8_t					control;
	uint8_t					config;
	int32_t					raw_temperature;
	int32_t					raw_pressure;
	int32_t					temperature;
	int32_t					pressure;
	struct bmp280_calib_param		calib_param;
};

#endif
