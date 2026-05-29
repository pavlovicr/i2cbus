/**
 * @file components/bmp280/include/bmp280.h
 * @brief Deklaracija funkcije za inicializacijo BMP280 senzorja na I2c vodilu.
 * 
 
 * 
 * @image   components/bmp280/images/bmp280.png
*/
#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

// I2C naslovi
#define BMP280_I2C_ADDR_0  0x76  // SDO povezan na GND
#define BMP280_I2C_ADDR_1  0x77  // SDO povezan na VDDIO

// Registri
#define BMP280_REG_ID       0xD0
#define BMP280_REG_RESET    0xE0
#define BMP280_REG_STATUS   0xF3
#define BMP280_REG_CTRL_MEAS 0xF4
#define BMP280_REG_CONFIG   0xF5
#define BMP280_REG_PRESS_MSB 0xF7
#define BMP280_REG_TEMP_MSB  0xFA

// ID vrednost (pričakovana)
#define BMP280_CHIP_ID      0x58

// Kalibracijski registri (začnejo pri 0x88)
#define BMP280_REG_CALIB    0x88

esp_err_t bmp280_init(i2c_master_bus_handle_t bus, i2c_master_dev_handle_t *dev, uint8_t i2c_addr);
esp_err_t bmp280_read(i2c_master_dev_handle_t dev, float *temperature, float *pressure);
esp_err_t bmp280_soft_reset(i2c_master_dev_handle_t dev);

