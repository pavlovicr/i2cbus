#include "i2c_esp32_master.h"
#include "esp_log.h"

static const char *TAG = "i2c_master";


/**
 * @brief Inicializira I2C master in vrne bus handle.
 * Nisem uporabil deklaracije i2c_master_bus_handle_t bus kot globalne spremenljivke,
 * ker je bolj modularno in varno, da se handle vrne preko funkcije.
 * V funkciji i2c_esp32_master_init() se ustvari I2C master bus in handle se vrne preko izhodnega parametra (output pointer) bus.
 *
 * @param bus Izhodni handle do I2C busa. 
 * Razlaga INPUT/OUTPUT: ““Parameter bus je izhodni parameter (output pointer), v katerega funkcija i2c_esp32_master_init() zapiše ustvarjen I2C bus handle. 
 * Klicatelj (npr. main.c) s tem pridobi referenco na inicializirano I2C vodilo.”
 * 
 * @return ESP_OK ob uspehu 
 */


esp_err_t i2c_esp32_master_init(i2c_master_bus_handle_t *bus)
{
    if (bus == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_master_bus_config_t cfg = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = 8,
        .scl_io_num = 9,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    esp_err_t ret = i2c_new_master_bus(&cfg, bus);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C init failed: %s", esp_err_to_name(ret));
    }

    return ret;
}