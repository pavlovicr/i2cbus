/**
* \image html shema I2C.jpg "shema I2C" height=200px   
 * @file components/i2c_esp32_master/include/i2c_esp32_master.h
 * @brief Konstante in funkcije za konfiguracijo in inicializacijo i2c vodila za ESP32 mikrokontroler.
 */

#pragma once


#include "driver/i2c_master.h" //esp-idf I2C master driver
#include "esp_err.h"

// Konstante – prilagodite jih svojim pinom
#define I2C_MASTER_NUM     I2C_NUM_0 // I2C port number
#define I2C_MASTER_SDA_IO  GPIO_NUM_8 // SDA pin
#define I2C_MASTER_SCL_IO  GPIO_NUM_9 // SCL pin
#define I2C_MASTER_FREQ_HZ 100000 // I2C frequency

// Deklaracije funkcij za inicializacijo in uporabo I2C master vodila

esp_err_t i2c_esp32_master_init(void);// Inicializacija I2C master vodila
i2c_master_bus_handle_t i2c_esp32_master_get_bus_handle(void);// Vrne ročaj do I2C master vodila
