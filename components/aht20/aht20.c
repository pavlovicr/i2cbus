// components/aht20/aht20.c
#include "aht20.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "AHT20";
#define AHT20_MEASUREMENT_DELAY_MS 80          // Čas, ki ga senzor potrebuje za meritev [ms], po specifikaciji.
#define I2C_TIMEOUT_MS 100                     // Timeout za I2C komunikacijo.

// Inicializacijska funkcija:
esp_err_t aht20_init(i2c_master_bus_handle_t bus_handle, i2c_master_dev_handle_t *dev_handle)
{
    if (bus_handle == NULL || dev_handle == NULL) {
        ESP_LOGE(TAG, "Invalid handle argument");
        return ESP_ERR_INVALID_ARG;
    }

    // 1. Dodaj napravo na I2C vodilo.
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = AHT20_I2C_ADDRESS,
        .scl_speed_hz = 100000,     // Hitrost 100 kHz deluje zanesljivo.
    };
    esp_err_t ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, dev_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add AHT20 device to bus: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "AHT20 device added to I2C bus.");

    // 2. Pošlji inicializacijski ukaz (0xBE, 0x08, 0x00).
    uint8_t init_cmd[3] = {AHT20_INIT_CMD, AHT20_INIT_PARAM_1, AHT20_INIT_PARAM_2};
    ret = i2c_master_transmit(*dev_handle, init_cmd, sizeof(init_cmd), I2C_TIMEOUT_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send initialization command: %s", esp_err_to_name(ret));
        return ret;
    }

    // 3. Počakaj, da senzor zaključi inicializacijo (po specifikaciji AHT20: do 10ms).
    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_LOGI(TAG, "AHT20 sensor initialized successfully.");
    return ESP_OK;
}

// Funkcija za branje temperature in vlažnosti:
esp_err_t aht20_read(i2c_master_dev_handle_t dev_handle, float *temperature, float *humidity)
{
    if (dev_handle == NULL || temperature == NULL || humidity == NULL) {
        ESP_LOGE(TAG, "Invalid argument");
        return ESP_ERR_INVALID_ARG;
    }

    // 1. Pošlji merilni ukaz (0xAC, 0x33, 0x00).
    uint8_t measure_cmd[3] = {AHT20_MEASURE_CMD, AHT20_MEASURE_PARAM_1, AHT20_MEASURE_PARAM_2};
    esp_err_t ret = i2c_master_transmit(dev_handle, measure_cmd, sizeof(measure_cmd), I2C_TIMEOUT_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send measurement command: %s", esp_err_to_name(ret));
        return ret;
    }

    // 2. Počakaj, da senzor zaključi meritev (minimalni čas 80ms po specifikaciji).
    vTaskDelay(pdMS_TO_TICKS(AHT20_MEASUREMENT_DELAY_MS));

    // 3. Preberi 6 bajtov odgovora, ki vsebujejo surove podatke in status.
    uint8_t raw_data[6];
    ret = i2c_master_receive(dev_handle, raw_data, sizeof(raw_data), I2C_TIMEOUT_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read measurement data: %s", esp_err_to_name(ret));
        return ret;
    }

    // [TESTIRANJE] Izpišemo surove podatke, da preverimo, ali senzor sploh odgovarja.
    ESP_LOGI(TAG, "Raw data: %02X %02X %02X %02X %02X %02X",
        raw_data[0], raw_data[1], raw_data[2], raw_data[3], raw_data[4], raw_data[5]);

    // Preveri status senzorja. Bit[7] (0x80) pomeni, da je senzor zaseden (BUSY).
    if (raw_data[0] & 0x80) {
        ESP_LOGW(TAG, "Sensor is busy, measurement may not be ready.");
        return ESP_ERR_INVALID_RESPONSE;
    }

    // 4. Pretvori surove podatke v uporabne vrednosti.
    // > Izračun surove vrednosti vlažnosti (20-bit).
    //  Vrstni red podatkov je: data[1] (4 biti) + data[2] (8 bitov) + data[3] (8 bitov).
    //  data[1] vsebuje v zgornjih 4-ih bitih podatke o temperaturi, v spodnjih 4-ih bitih pa podatke o vlažnosti.
    uint32_t raw_humidity = ((uint32_t)(raw_data[1] & 0x0F) << 16) |  // Zgornji 4 biti podatka o vlažnosti.
                            ((uint32_t)raw_data[2] << 8) |            // Srednjih 8 bitov podatka o vlažnosti.
                            ((uint32_t)raw_data[3]);                  // Spodnjih 8 bitov podatka o vlažnosti.

    // > Izračun surove vrednosti temperature (20-bit).
    //  data[1] vsebuje v zgornjih 4-ih bitih podatke o temperaturi, v spodnjih 4-ih bitih pa podatke o vlažnosti,
    //  zato je treba najprej pridobiti zgornje 4 bite iz data[1].
    uint32_t raw_temperature = ((uint32_t)((raw_data[1] & 0xF0) >> 4) << 16) | // Zgornji 4 biti podatka o temperaturi.
                               ((uint32_t)raw_data[4] << 8) |                  // Srednjih 8 bitov podatka o temperaturi.
                               ((uint32_t)raw_data[5]);                         // Spodnjih 8 bitov podatka o temperaturi.

    // 5. Pretvorba v fizikalne enote po formuli iz AHT20 datasheeta.
    *humidity = (raw_humidity * 100.0f) / 1048576.0f;          // 2^20 = 1048576
    *temperature = (raw_temperature * 200.0f) / 1048576.0f - 50.0f;

    // Preverimo, ali so vrednosti v pričakovanem območju.
    if (*temperature < -40.0f || *temperature > 85.0f) {
        ESP_LOGW(TAG, "Temperature value out of range: %.2f °C", *temperature);
    }
    if (*humidity < 0.0f || *humidity > 100.0f) {
        ESP_LOGW(TAG, "Humidity value out of range: %.2f %%", *humidity);
    }

    return ESP_OK;
}