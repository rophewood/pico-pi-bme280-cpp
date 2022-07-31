#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "BME280.hpp"

#ifdef PICO_DEFAULT_LED_PIN
#define LED_PIN PICO_DEFAULT_LED_PIN
#endif

int main()
{
    stdio_init_all(); // Initialise I/O

#ifdef LED_PIN
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
#endif

    // BME280
    Sensors::BME280 bme280;
    const uint sda_pin = 4;
    const uint scl_pin = 5;
    const uint baudrate = 400 * 1000;
    bool is_bme280_ok = bme280.init(i2c0, Sensors::I2C_BME280_ADDR, sda_pin, scl_pin, baudrate);

    if (is_bme280_ok)
    {
        double temp, press, hum;
        uint8_t led_state = 1;
        while(1)
        {
            #ifdef LED_PIN
                // Blink led
                gpio_put(LED_PIN, led_state);
                led_state = (led_state == 1)?0:1;
            #endif

            // Convert and display results
            bme280.execute_conversion();
            temp = bme280.getTemperature();
            press = bme280.getAirPressure();
            hum = bme280.getAirHumidity();
            printf("%0.2lf deg C, %0.2lf hPa, %0.2lf%%\n", temp, press, hum);
            
            sleep_ms(2000);
        }
    }
    else
    {
        printf("BME280 is not working!!\n");
    }

    return 0;
}
