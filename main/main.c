#include "i2c_esp32_master.h"
#include "aht20.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    i2c_master_bus_handle_t bus;
    i2c_master_dev_handle_t aht;

    float t, h;

    // 1. Ustvari I2C bus (main je lastnik)
    i2c_esp32_master_init(&bus);

    // 2. Dodaj senzor na bus
    aht20_init(bus, &aht);

    aht20_check(aht);     // Preveri, ali je senzor kalibriran in prisoten

    

    // 3. Periodično branje
    while (1) {
        aht20_read(aht, &t, &h);

        ESP_LOGI(TAG, "AHT20 Temperatura = %.2f°C AHT20 Vlaga =%.2f%%", t, h);
        

        vTaskDelay(pdMS_TO_TICKS(2000));
    }


}