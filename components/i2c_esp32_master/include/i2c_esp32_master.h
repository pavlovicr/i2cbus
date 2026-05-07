/**
 * @file components/i2c_esp32_master/include/i2c_esp32_master.h
 * @brief Deklaracija funkcije za inicializacijo I2C mastera na ESP32 mikrokontrolerju.
 * 
 * Komponenta omogoča enostavno postavitev I2C vodila z master mikrokontrolerjem esp32s3.
 * V c fajlu so definirani parametri za I2C vodilo, kot so pinout, hitrost, in druge nastavitve in sicer :
 * - I2C port: I2C_NUM_0 (na ESP32S3 je na voljo več I2C portov, tukaj uporabljamo prvi)
 * - SDA pin: GPIO8    
 * - SCL pin: GPIO9
 * - Hitrost: 100kHz
 * - Notranji pull-up upor: Omogočen
 * - Glitch ignore count: 7 
 * 
 * V kolikor bi želeli spremeniti te nastavitve, jih lahko prilagodimo v funkciji i2c_esp32_master_init() v i2c_esp32_master.c fajlu.
 * 
 * 
 * @image   components/i2c_esp32_master/images/shema I2C.png    
 */

#pragma once 

#include "esp_err.h"
#include "driver/i2c_master.h"

/**
 * @brief Inicializira I2C master in vrne bus handle
 *
 * @param bus Izhodni handle do I2C busa
 * @return ESP_OK ob uspehu
 */
esp_err_t i2c_esp32_master_init(i2c_master_bus_handle_t *bus);