/**
* \image html shema I2C.jpg "shema I2C" height=200px   
 * @file main/main.c
 * @brief postavitev i2c naprav na master vodilo z esp32 mikrokontrolerjem,
 
 
 
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "i2c_esp32_master.h"
#include "aht20.h"

static const char *TAG = "MAIN";



void app_main(void)
{
    // 1. Inicializacija I2C vodila preko naše komponente.
    ESP_ERROR_CHECK(i2c_esp32_master_init());

    // 2. Pridobitev ročaja za I2C vodilo.
    i2c_master_bus_handle_t bus_handle = i2c_esp32_master_get_bus_handle();
    if (bus_handle == NULL) {
        ESP_LOGE(TAG, "Failed to get I2C bus handle");
        return;
    }
    // 3. Inicializiraj AHT20 senzor. To bo zanjo ustvarilo tudi ročaj.
    i2c_master_dev_handle_t aht20_handle = NULL;
    ESP_ERROR_CHECK(aht20_init(bus_handle, &aht20_handle));

    float temperature, humidity;
    while (1) {
        // 4. Periodično preberi podatke s senzorja.
        esp_err_t ret = aht20_read(aht20_handle, &temperature, &humidity);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "=== AHT20 Sensor ===");
            ESP_LOGI(TAG, "Temperature: %.2f °C", temperature);
            ESP_LOGI(TAG, "Humidity: %.2f %%", humidity);
        } else {
            ESP_LOGE(TAG, "Failed to read from AHT20: %s", esp_err_to_name(ret));
        }
        vTaskDelay(pdMS_TO_TICKS(2000)); // Počakaj 2 sekundi pred naslednjo meritvijo.
    }
}