/**
 * @file components/aht20/aht20.c
 * @brief Implementacija funkcije za inicializacijo AHT20 senzorja na I2c vodilu.
 * 
 * Koda vsebuje dve funkciji:
 * - aht20_init(): Inicializira AHT20 senzor na I2C vodilu z določenimi parametri, kot so naslov in hitrost komunikacije.
 * - aht20_read(): Pošlje ukaz za branje podatkov s senzorja, počaka 80ms, prebere surove podatke in jih pretvori v temperaturo in vlago.   
 * 
 * fUNKCIJA aht20_init() nastavi naslednje parametre za komunikacijo s senzorjem:
 * - I2C naslov: 0x38
 * - Hitrost komunikacije: 100 kHz
 * 
 * fUNKCIJA aht20_read() izvede naslednje korake:
 * - Pošlje ukaz za branje podatkov s senzorja: 0xAC, 0x33, 0x00
 * - Počaka 80ms, da senzor obdela ukaz
 * - Prebere 6 bajtov surovih podatkov s senzorja
 * - Iz surovih podatkov izračuna temperaturo in vlago:
        * - Vlaga (%) = (h * 100.0) / 1048576.0
        * - Temperatura (°C) = (t * 200.0) / 1048576.0 - 50.0           
 * 
 * @image   components/aht20/images/aht20.png
 * https://files.seeedstudio.com/wiki/Grove-AHT20_I2C_Industrial_Grade_Temperature_and_Humidity_Sensor/AHT20-datasheet-2020-4-16.pdf
*/


#include "aht20.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

esp_err_t aht20_init(i2c_master_bus_handle_t bus,
                     i2c_master_dev_handle_t *dev)
{
    i2c_device_config_t cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x38,
        .scl_speed_hz = 100000,
    };

    return i2c_master_bus_add_device(bus, &cfg, dev);
}

esp_err_t aht20_read(i2c_master_dev_handle_t dev,
                     float *temperature,
                     float *humidity)
{
    uint8_t cmd[] = {0xAC, 0x33, 0x00};

    i2c_master_transmit(dev, cmd, sizeof(cmd), 100);
    vTaskDelay(pdMS_TO_TICKS(80));

    uint8_t data[6];
    i2c_master_receive(dev, data, 6, 100);

    uint32_t h =
        ((data[1] & 0x0F) << 16) | (data[2] << 8) | data[3];

    uint32_t t =
        ((data[1] & 0xF0) >> 4 << 16) | (data[4] << 8) | data[5];

    *humidity = (h * 100.0f) / 1048576.0f;
    *temperature = (t * 200.0f) / 1048576.0f - 50.0f;

    return ESP_OK;
}