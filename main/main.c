#include "i2c_esp32_master.h"
#include "aht20.h"
#include "bmp280.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    i2c_master_bus_handle_t bus;
    i2c_master_dev_handle_t aht;
    

    float t, h;
    float temperature, pressure;

    // 1. Ustvari I2C bus (main je lastnik)
    i2c_esp32_master_init(&bus);
    // 2. Dodaj senzor na bus
//.........................................AHT20.................................................
    aht20_init(bus, &aht);
    aht20_check(aht);     // Preveri, ali je senzor kalibriran in prisoten
//..........................................BMP280.................................................
    // Inicializacija BMP280 (naslov 0x76 ali 0x77)
    i2c_master_dev_handle_t bmp280_dev;
    ESP_ERROR_CHECK(bmp280_init(bus, &bmp280_dev, BMP280_I2C_ADDR_1)); // 0x77
    

    // 3. Periodično branje
    while (1) {
        aht20_read(aht, &t, &h);
        ESP_LOGI(TAG, "AHT20 Temperatura = %.2f°C AHT20 Vlaga =%.2f%%", t, h);
        
        bmp280_read(bmp280_dev, &temperature, &pressure);
        ESP_LOGI(TAG, "BMP280 Temperatura = %.2f°C BMP280 Tlak = %.2f hPa", temperature, pressure);

        vTaskDelay(pdMS_TO_TICKS(2000));  // počakaj 2 sekundi
    }

}
