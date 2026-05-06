/**
* \image html aht20.jpg "aht20" height=200px   
 * @file aht20.h
 * @brief Vmesnik za komunikacijo z AHT20 senzorjem temperature in vlage preko I2C.
 */

#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

/**
 * @brief Konstante, specifične za AHT20 senzor
 */
#define AHT20_I2C_ADDRESS          0x38    /*!< I2C naslov naprave */
#define AHT20_INIT_CMD             0xBE    /*!< Ukaz za inicializacijo */
#define AHT20_INIT_PARAM_1         0x08    /*!< Prvi parameter inicializacije */
#define AHT20_INIT_PARAM_2         0x00    /*!< Drugi parameter inicializacije */
#define AHT20_MEASURE_CMD          0xAC    /*!< Ukaz za meritev */
#define AHT20_MEASURE_PARAM_1      0x33    /*!< Prvi parameter meritve */
#define AHT20_MEASURE_PARAM_2      0x00    /*!< Drugi parameter meritve */
#define AHT20_READ_STATUS_CMD      0x71    /*!< Ukaz za branje statusnega registra */
#define AHT20_SOFT_RESET_CMD       0xBA    /*!< Ukaz za mehko ponastavitev senzorja */

/**
 * @brief Inicializira AHT20 senzor na danem I2C vodilu.
 *
 * @param bus_handle Ročaj (handle) I2C vodila, ki ga vrne vaša komponenta i2c_esp32_master.
 * @param dev_handle Kazalec na spremenljivko, kamor se shrani ročaj naprave za nadaljnjo uporabo.
 * @return ESP_OK ob uspehu, sicer koda napake.
 */
esp_err_t aht20_init(i2c_master_bus_handle_t bus_handle, i2c_master_dev_handle_t *dev_handle);

/**
 * @brief Prebere temperaturo in relativno vlažnost iz AHT20 senzorja.
 *
 * @param dev_handle Veljaven ročaj naprave, pridobljen s klicem aht20_init().
 * @param temperature Kazalec na spremenljivko float za shranitev temperature v stopinjah Celzija.
 * @param humidity Kazalec na spremenljivko float za shranitev relativne vlažnosti v procentih (%).
 * @return ESP_OK ob uspehu, sicer koda napake.
 */
esp_err_t aht20_read(i2c_master_dev_handle_t dev_handle, float *temperature, float *humidity);

