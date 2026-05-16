/**
 * @file components/aht20/include/aht20.h
 * @brief Deklaracija funkcije za inicializacijo AHT20 senzorja na I2c vodilu.
 * 
 * Komponenta omogoča enostavno inicializacijo in branje podatkov s senzorja AHT20, ki meri temperaturo in vlago.
 * V c fajlu so definirani parametri za komunikacijo s senzorjem, kot so :
 * - I2C naslov: 0x38
 * - Ukaz za branje: 0xAC, 0x33, 0x00
 * - Časovni zamik po ukazu: 80ms
 * - Izračun temperature in vlage iz prebranih podatkov:
 *  - Vlaga (%) = (h * 100.0) / 1048576.0
 * - Temperatura (°C) = (t * 200.0) / 1048576.0 - 50.0
 *
 *  V kolikor bi želeli spremeniti te nastavitve, jih lahko prilagodimo v funkciji aht20_init() in aht20_read() v aht20.c fajlu.
 * 
 * @image   components/aht20/images/aht20.png
*/
#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

/**
 * @brief Inicializira AHT20 senzor
 */
esp_err_t aht20_init(i2c_master_bus_handle_t bus,
                     i2c_master_dev_handle_t *dev);

/**
 * @brief Prebere temperaturo in vlago
 */
esp_err_t aht20_read(i2c_master_dev_handle_t dev,
                     float *temperature,
                     float *humidity);

/**
 * @brief Kalibracija senzorja, če je potrebno
 */
esp_err_t aht20_calibrate(i2c_master_dev_handle_t dev);             
/**
 * @brief Preveri, ali je senzor prisoten in deluje in ali je kalibriran. Če senzor ni kalibriran, ga kalibrira.
 */
esp_err_t aht20_check(i2c_master_dev_handle_t dev);

