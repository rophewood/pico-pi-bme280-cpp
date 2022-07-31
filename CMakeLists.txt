cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(bme280_example C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
 )

add_subdirectory(BME280)
add_executable(${PROJECT_NAME}
        ${SOURCES}
        ${HEADERS}
        main.cpp
 )

# Add pico_multicore which is required for multicore functionality
target_link_libraries(${PROJECT_NAME}
        pico_stdlib
        hardware_i2c
)
        
target_include_directories(${PROJECT_NAME} PUBLIC 
        BME280
)

# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)