// my_class.h
#ifndef BME280_H // include guard
#define BME280_H

#include "hardware/i2c.h"
#include "BME280_driver/bme280.h"

namespace Sensors
{
    const uint8_t I2C_BME280_ADDR = 0x76;

    struct I2c_config
    {
        i2c_inst_t* i2c;
        uint8_t dev_addr;
    };
    
    class BME280
    {
    public:
        bool init(i2c_inst_t* i2c, uint8_t dev_addr, uint sda_pin, uint scl_pin, uint baudrate);
        bool getIsOk() { return is_ok; }
        bool execute_conversion();

        double getTemperature() { return 0.01f * comp_data.temperature; } 
        int32_t getTemperatureRaw() { return comp_data.temperature; } 
        double getAirPressure() { return 0.0001f * comp_data.pressure; } 
        uint32_t getAirPressureRaw() { return comp_data.pressure; } 
        double getAirHumidity() { return 1.0f / 1024.0f * comp_data.humidity; } 
        uint32_t getAirHumidityRaw() { return comp_data.humidity; } 

        static void user_delay_us(uint32_t period, void *intf_ptr);
        static int8_t user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
        static int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
    
    private:
        struct I2c_config i2c_config;
        uint8_t dev_addr;
        uint sda_pin;
        uint scl_pin;
        uint baudrate;

        int8_t rslt;
        bool is_ok;

        struct bme280_dev dev;
        struct bme280_data comp_data;

    };
}

#endif /* BME280_H */
