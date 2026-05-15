/**
 * @file components/bmp280/bmp280.c
 * @brief Implementacija funkcije za inicializacijo BMP280 senzorja na I2c vodilu.
 * 
 * Koda vsebuje dve funkciji:
 * - bmp280_init(): Inicializira BMP280 senzor na I2C vodilu z določenimi parametri, kot so naslov in hitrost komunikacije.
 * - bmp280_read(): Pošlje ukaz za branje podatkov s senzorja, počaka 80ms, prebere surove podatke in jih pretvori v temperaturo in tlak.   
 * 
 * fUNKCIJA bmp280_init() nastavi naslednje parametre za komunikacijo s senzorjem:
 * - I2C naslov: 0x58
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
 * @image   components/bmp280/images/bmp280.png
 * https://files.seeedstudio.com/wiki/Grove-BMP280_I2C_Industrial_Grade_Temperature_and_Pressure_Sensor/BMP280-datasheet-2020-4-16.pdf
*/


#include "bmp280.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

esp_err_t bmp280_init(i2c_master_bus_handle_t bus,
                                 i2c_master_dev_handle_t *dev)
{
    i2c_device_config_t cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x77,
        .scl_speed_hz = 100000,
    };

    return i2c_master_bus_add_device(bus, &cfg, dev);
}

esp_err_t bmp280_read(i2c_master_dev_handle_t dev,
                     float *temperature,
                     float *pressure        )
{
    uint8_t cmd[] = {0xAC, 0x33, 0x00}; // Ukaz za branje podatkov s senzorja

    i2c_master_transmit(dev, cmd, sizeof(cmd), 100);// Pošljemo ukaz za branje podatkov s senzorja
    vTaskDelay(pdMS_TO_TICKS(80));

    uint8_t data[6];
    i2c_master_receive(dev, data, 6, 100);// Preberemo 6 bajtov surovih podatkov s senzorja

    uint32_t h =
        ((data[1] << 16) | (data[2] << 8) | data[3]) >> 4;

    uint32_t t =
        (((data[3] & 0x0F) << 16) | (data[4] << 8) | data[5]);
    *pressure  = (h * 100.0f) / 1048576.0f;
    *temperature = (t * 200.0f) / 1048576.0f - 50.0f;


    // Debug: Izpišemo surove podatke
    printf("%02X %02X %02X %02X %02X %02X\n",
       data[0], data[1], data[2],
       data[3], data[4], data[5]);


    return ESP_OK;
}
