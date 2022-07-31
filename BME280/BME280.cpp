#include <stdio.h>
#include "pico/stdlib.h"
#include "BME280.hpp"

void Sensors::BME280::user_delay_us(uint32_t period, void *intf_ptr)
{
    sleep_us(period);
}

int8_t Sensors::BME280::user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    Sensors::I2c_config* i2c_config = (Sensors::I2c_config*)intf_ptr;
    i2c_inst_t* i2c = i2c_config->i2c;
    uint8_t dev_addr = i2c_config->dev_addr;
    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

    /*
     * The parameter intf_ptr can be used as a variable to store the I2C address of the device
     */

    /*
     * Data on the bus should be like
     * |------------+---------------------|
     * | I2C action | Data                |
     * |------------+---------------------|
     * | Start      | -                   |
     * | Write      | (reg_addr)          |
     * | Stop       | -                   |
     * | Start      | -                   |
     * | Read       | (reg_data[0])       |
     * | Read       | (....)              |
     * | Read       | (reg_data[len - 1]) |
     * | Stop       | -                   |
     * |------------+---------------------|
     */

    //printf("read i2c\n");
    i2c_write_blocking(i2c, dev_addr, &reg_addr, 1, true);
    i2c_read_blocking(i2c, dev_addr, reg_data, len, false);

    return rslt;
}

int8_t Sensors::BME280::user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    Sensors::I2c_config* i2c_config = (Sensors::I2c_config*)intf_ptr;
    i2c_inst_t* i2c = i2c_config->i2c;
    uint8_t dev_addr = i2c_config->dev_addr;
    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */
    uint8_t msg[len + 1];

    /*
     * The parameter intf_ptr can be used as a variable to store the I2C address of the device
     */

    /*
     * Data on the bus should be like
     * |------------+---------------------|
     * | I2C action | Data                |
     * |------------+---------------------|
     * | Start      | -                   |
     * | Write      | (reg_addr)          |
     * | Write      | (reg_data[0])       |
     * | Write      | (....)              |
     * | Write      | (reg_data[len - 1]) |
     * | Stop       | -                   |
     * |------------+---------------------|
     */

    //printf("write i2c len=%d\n",len);

    // Append register address to front of data packet
    msg[0] = reg_addr;
    for (uint32_t i = 0; i < len; i++) {
        msg[i + 1] = reg_data[i];
    }

    i2c_write_blocking(i2c, dev_addr, msg, len + 1, false);

    return rslt;
}

bool Sensors::BME280::init(i2c_inst_t* i2c, uint8_t dev_addr, uint sda_pin, uint scl_pin, uint baudrate)
{
    is_ok = false;
    i2c_config.i2c = i2c;
    i2c_config.dev_addr = dev_addr;
    this->sda_pin = sda_pin;
    this->scl_pin = scl_pin;
    this->baudrate = baudrate;

    //Initialize I2C port at baudrate kHz
    i2c_init(i2c, baudrate);

    // Initialize I2C pins
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
    
    rslt = BME280_OK;

    dev.intf_ptr = &i2c_config;
    dev.intf = BME280_I2C_INTF;
    dev.read = user_i2c_read;
    dev.write = user_i2c_write;
    dev.delay_us = user_delay_us;

    rslt = bme280_init(&dev);

    if (rslt != BME280_OK)
    {
        printf("Bme280 init failed (code %+d).", rslt);
        return false;
    }

    uint8_t settings_sel;

    /* Recommended mode of operation: Indoor navigation */
    dev.settings.osr_h = BME280_OVERSAMPLING_1X;
	dev.settings.osr_p = BME280_OVERSAMPLING_16X;
	dev.settings.osr_t = BME280_OVERSAMPLING_2X;
	dev.settings.filter = BME280_FILTER_COEFF_16;

	settings_sel = BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_FILTER_SEL;
	rslt = bme280_set_sensor_settings(settings_sel, &dev);
    if (rslt != BME280_OK)
    {
        printf("Failed to set sensor settings (code %+d).", rslt);
        return false;
    }
    is_ok = true;
    return true;
}

bool Sensors::BME280::execute_conversion()
{
    is_ok = false;
    rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
    if (rslt != BME280_OK)
    {
        printf("Failed to set sensor mode (code %+d).", rslt);
        return false;
    }

    /* Wait for the measurement to complete and print data @25Hz */
    //dev.delay_us(40000, dev.intf_ptr);

    // Use the calculated delay before converting.
    uint32_t delay = bme280_cal_meas_delay(&dev.settings);
    dev.delay_us(delay, dev.intf_ptr); 

    rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
    if (rslt != BME280_OK)
    {
        printf("Failed to get sensor data (code %+d).", rslt);
       return false;
    }
    
    //double temperature = 0.01f * comp_data.temperature;
    //double pressure = 0.0001f * comp_data.pressure;
    //double humidity = 1.0f / 1024.0f * comp_data.humidity;
    //printf("%0.2lf deg C, %0.2lf hPa, %0.2lf%%\n", temperature, pressure, humidity);

    is_ok = true;
    return true;
}