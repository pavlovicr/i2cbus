/**
 * @file components/bmp280/bmp280.c
 * @brief Implementacija funkcije za inicializacijo BMP280 senzorja na I2c vodilu.
 * 
 *  
 * 
 * @image   components/bmp280/images/bmp280.png
 * https://files.seeedstudio.com/wiki/Grove-BMP280_I2C_Industrial_Grade_Temperature_and_Pressure_Sensor/BMP280-datasheet-2020-4-16.pdf
*/


// components/bmp280/bmp280.c
#include "bmp280.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "BMP280";

// Kalibracijske konstante in vmesne spremenljivke za kompenzacijo
static struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
    int32_t  t_fine;  // vmesna spremenljivka za kompenzacijo temperature, potrebna tudi za tlak
} bmp280_cal;

// Funkcija za branje registra (8-bit)
static esp_err_t read_register(i2c_master_dev_handle_t dev, uint8_t reg, uint8_t *data)
{
    return i2c_master_transmit_receive(dev, &reg, 1, data, 1, 100);
}

// Funkcija za branje več registrov
static esp_err_t read_registers(i2c_master_dev_handle_t dev, uint8_t reg, uint8_t *data, size_t len)
{
    return i2c_master_transmit_receive(dev, &reg, 1, data, len, 100);
}

// Funkcija za pisanje v register (8-bit)
static esp_err_t write_register(i2c_master_dev_handle_t dev, uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = {reg, value};
    return i2c_master_transmit(dev, buf, sizeof(buf), 100);
}

// Branje 16-bitne vrednosti (little-endian)
static uint16_t read_u16_le(uint8_t *buf)
{
    return (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
}

// Branje 16-bitne vrednosti (signed, little-endian)
static int16_t read_s16_le(uint8_t *buf)
{
    return (int16_t)read_u16_le(buf);
}

// Branje kalibracijskih konstant iz senzorja
static esp_err_t bmp280_read_calibration(i2c_master_dev_handle_t dev)
{
    uint8_t calib_data[24];  // 24 bajtov kalibracijskih podatkov
    esp_err_t ret = read_registers(dev, BMP280_REG_CALIB, calib_data, sizeof(calib_data));
    if (ret != ESP_OK) return ret;
    
    // Temperaturne konstante (dig_T1, dig_T2, dig_T3)
    bmp280_cal.dig_T1 = read_u16_le(&calib_data[0]);
    bmp280_cal.dig_T2 = read_s16_le(&calib_data[2]);
    bmp280_cal.dig_T3 = read_s16_le(&calib_data[4]);
    
    // Tlačne konstante (dig_P1 .. dig_P9)
    bmp280_cal.dig_P1 = read_u16_le(&calib_data[6]);
    bmp280_cal.dig_P2 = read_s16_le(&calib_data[8]);
    bmp280_cal.dig_P3 = read_s16_le(&calib_data[10]);
    bmp280_cal.dig_P4 = read_s16_le(&calib_data[12]);
    bmp280_cal.dig_P5 = read_s16_le(&calib_data[14]);
    bmp280_cal.dig_P6 = read_s16_le(&calib_data[16]);
    bmp280_cal.dig_P7 = read_s16_le(&calib_data[18]);
    bmp280_cal.dig_P8 = read_s16_le(&calib_data[20]);
    bmp280_cal.dig_P9 = read_s16_le(&calib_data[22]);
    
    ESP_LOGI(TAG, "Calibration data read: T1=%u, T2=%d, T3=%d", 
             bmp280_cal.dig_T1, bmp280_cal.dig_T2, bmp280_cal.dig_T3);
    return ESP_OK;
}

// Izračun temperature iz surovih podatkov (po Bosch formuli)
static int32_t bmp280_compensate_temp(int32_t adc_temp)
{
    int32_t var1, var2, T;
    var1 = ((((adc_temp >> 3) - ((int32_t)bmp280_cal.dig_T1 << 1))) *
            ((int32_t)bmp280_cal.dig_T2)) >> 11;
    var2 = (((((adc_temp >> 4) - ((int32_t)bmp280_cal.dig_T1)) *
              ((adc_temp >> 4) - ((int32_t)bmp280_cal.dig_T1))) >> 12) *
            ((int32_t)bmp280_cal.dig_T3)) >> 14;
    bmp280_cal.t_fine = var1 + var2;
    T = (bmp280_cal.t_fine * 5 + 128) >> 8;
    return T;
}

// Izračun tlaka iz surovih podatkov (po Bosch formuli)
static uint32_t bmp280_compensate_press(int32_t adc_press)
{
    int64_t var1, var2, p;
    var1 = ((int64_t)bmp280_cal.t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)bmp280_cal.dig_P6;
    var2 = var2 + ((var1 * (int64_t)bmp280_cal.dig_P5) << 17);
    var2 = var2 + (((int64_t)bmp280_cal.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)bmp280_cal.dig_P3) >> 8) +
           ((var1 * (int64_t)bmp280_cal.dig_P2) << 12);
    var1 = ((((int64_t)1) << 47) + var1) * ((int64_t)bmp280_cal.dig_P1) >> 33;
    if (var1 == 0) return 0;
    p = 1048576 - adc_press;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = ((int64_t)bmp280_cal.dig_P9) * (p >> 13) * (p >> 13) >> 25;
    var2 = ((int64_t)bmp280_cal.dig_P8) * p >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)bmp280_cal.dig_P7) << 4);
    return (uint32_t)p;
}

// Soft reset senzorja
esp_err_t bmp280_soft_reset(i2c_master_dev_handle_t dev)
{
    esp_err_t ret = write_register(dev, BMP280_REG_RESET, 0xB6);
    if (ret != ESP_OK) return ret;
    vTaskDelay(pdMS_TO_TICKS(50));  // počakaj, da se senzor resetira
    return ESP_OK;
}

// Inicializacija BMP280
esp_err_t bmp280_init(i2c_master_bus_handle_t bus, i2c_master_dev_handle_t *dev, uint8_t i2c_addr)
{
    // 1. Dodaj napravo na vodilo
    i2c_device_config_t cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = i2c_addr,
        .scl_speed_hz = 100000,
    };
    esp_err_t ret = i2c_master_bus_add_device(bus, &cfg, dev);
    if (ret != ESP_OK) return ret;
    
    // 2. Preveri chip ID
    uint8_t chip_id;
    ret = read_register(*dev, BMP280_REG_ID, &chip_id);
    if (ret != ESP_OK) return ret;
    
    if (chip_id != BMP280_CHIP_ID) {
        ESP_LOGE(TAG, "Wrong chip ID: 0x%02X (expected 0x58)", chip_id);
        return ESP_ERR_NOT_FOUND;
    }
    ESP_LOGI(TAG, "BMP280 found, chip ID: 0x%02X", chip_id);
    
    // 3. Soft reset (opcijsko, priporočljivo)
    ret = bmp280_soft_reset(*dev);
    if (ret != ESP_OK) return ret;
    
    // 4. Preberi kalibracijske konstante
    ret = bmp280_read_calibration(*dev);
    if (ret != ESP_OK) return ret;
    
    // 5. Konfiguriraj senzor: oversampling, način delovanja
    // CTRL_MEAS: os_tmp=2 (x2), os_pres=5 (x16), mode=3 (normal mode)
    ret = write_register(*dev, BMP280_REG_CTRL_MEAS, 0x57);  // 0b01010111
    if (ret != ESP_OK) return ret;
    
    // CONFIG: filter=4 (x16), standby=5 (1000ms)
    ret = write_register(*dev, BMP280_REG_CONFIG, 0xA0);  // 0b10100000
    if (ret != ESP_OK) return ret;
    
    vTaskDelay(pdMS_TO_TICKS(50));  // počakaj, da se prve meritve izvedejo
    
    ESP_LOGI(TAG, "BMP280 initialized successfully");
    return ESP_OK;
}

// Branje temperature in tlaka
esp_err_t bmp280_read(i2c_master_dev_handle_t dev, float *temperature, float *pressure)
{
    if (dev == NULL || temperature == NULL || pressure == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // Preberi 6 bajtov: tlak MSB, LSB, xlsb (3) + temperatura MSB, LSB, xlsb (3)
    uint8_t data[6];
    esp_err_t ret = read_registers(dev, BMP280_REG_PRESS_MSB, data, sizeof(data));
    if (ret != ESP_OK) return ret;
    
    // Sestavi 20-bitne surove podatke (tlak)
    int32_t adc_press = (int32_t)(((uint32_t)data[0] << 12) | 
                                   ((uint32_t)data[1] << 4) | 
                                   (data[2] >> 4));
    
    // Sestavi 20-bitne surove podatke (temperatura)
    int32_t adc_temp = (int32_t)(((uint32_t)data[3] << 12) | 
                                  ((uint32_t)data[4] << 4) | 
                                  (data[5] >> 4));
    
    // Kompenzacija temperature (dobiš temperaturo v 0.01°C)
    int32_t temp_comp = bmp280_compensate_temp(adc_temp);
    *temperature = (float)temp_comp / 100.0f;
    
    // Kompenzacija tlaka (dobiš tlak v Pa)
    uint32_t press_comp = bmp280_compensate_press(adc_press);
    *pressure = (float)press_comp / 100.0f;  // pretvori v hPa
    
    ESP_LOGD(TAG, "Raw: press=0x%05X (%d), temp=0x%05X (%d)", 
             adc_press, adc_press, adc_temp, adc_temp);
    ESP_LOGD(TAG, "Comp: T=%.2f°C, P=%.2f hPa", *temperature, *pressure);
    
    return ESP_OK;
}