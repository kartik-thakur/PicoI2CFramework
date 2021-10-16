cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(BMP280 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_executable(bmp280
	i2c_core.c
	i2c_bus.c
	i2c_dev.c
        bmp280.c
        )

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(bmp280
	pico_stdlib
	hardware_i2c
)

# create map/bin/hex file etc.
pico_add_extra_outputs(bmp280)
pico_enable_stdio_usb(bmp280 1)
