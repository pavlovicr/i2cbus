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
    i2c_master_dev_handle_t bmp;

    float t, h, t1,p;

    // 1. Ustvari I2C bus (main je lastnik)
    i2c_esp32_master_init(&bus);

    // 2. Dodaj senzor na bus
    aht20_init(bus, &aht);
    bmp280_init(bus, &bmp);

    // 3. Periodično branje
    
    while (1) {
        aht20_read(aht, &t, &h);
        bmp280_read(bmp, &t1, &p);

        ESP_LOGI(TAG, "T=%.2f°C P=%.2f Pa", t1, p );

        vTaskDelay(pdMS_TO_TICKS(2000));
    }


}
