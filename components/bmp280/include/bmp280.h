/**
 * @file components/bmp280/include/bmp280.h
 * @brief Deklaracija funkcije za inicializacijo BMP280 senzorja na I2c vodilu.
 * 
 * Komponenta omogoča enostavno inicializacijo in branje podatkov s senzorja BMP280, ki meri temperaturo in tlak.
 * V c fajlu so definirani parametri za komunikacijo s senzorjem, kot so :
 * - I2C naslov: 0x77
 * - Ukaz za branje: 0xAC, 0x33, 0x00
 * - Časovni zamik po ukazu: 80ms
 * - Izračun temperature in tlaka iz prebranih podatkov:
 *  - Tlak (Pa) = (p * 100.0) / 1048576.0
 * - Temperatura (°C) = (t * 200.0) / 1048576.0 - 50.0
 *
 *  V kolikor bi želeli spremeniti te nastavitve, jih lahko prilagodimo v funkciji bmp280_init() in bmp280_read() v bmp280.c fajlu.
 * 
 * @image   components/bmp280/images/bmp280.png
*/
#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

/**
 * @brief Inicializira BMP280 senzor
 */
esp_err_t bmp280_init(i2c_master_bus_handle_t bus,
                     i2c_master_dev_handle_t *dev);

/**
 * @brief Prebere temperaturo in tlak s BMP280 senzorja
 */
esp_err_t bmp280_read(i2c_master_dev_handle_t dev,
                     float *temperature,
                     float *pressure);