#include "i2c_core.h"
#include "i2c_bus.h"
#include "pico/stdlib.h"

static struct i2c_bus i2c_buses[] = {
	{
		.bus		= i2c0,
		.baudrate	= 100000,
		.sda_gpio	= PICO_DEFAULT_I2C_SDA_PIN,
		.scl_gpio	= PICO_DEFAULT_I2C_SDA_PIN,
	},
};

static void i2c_bus_probe(struct i2c_bus *bus)
{
	uint8_t device_addr;

	/*
	 * Initialize I2C bus.
	 */
	gpio_set_function(bus->sda_gpio, GPIO_FUNC_I2C);
	gpio_set_function(bus->scl_gpio, GPIO_FUNC_I2C);
	i2c_init(bus->bus, bus->baudrate);

	/*
	 * Scan for I2C devices connected to the bus and call their
	 * driver probe if registered.
	 */
	for (device_addr = 0x0; device_addr <= 0x80; device_addr++) {
		uint8_t err, value;

		if (!is_reserved_addr(device_addr)) {
			err = i2c_read_blocking(bus->bus, device_addr, &value,
						1, false);
			if (err < 0)
				continue;

			i2c_device_try_probe(bus, device_addr);
		}
	}
}

void i2c_bus_init()
{
	unsigned int i;
	unsigned int num_buses;

	num_buses = sizeof(i2c_buses) / sizeof(struct i2c_bus);

	for (i = 0; i < num_buses; i++)
		i2c_bus_probe(&i2c_buses[i]);
}
