/**
 * @file components/aht20/aht20.c
 * @brief Implementacija funkcije za inicializacijo AHT20 senzorja na I2c vodilu.
 * 
 * Koda vsebuje štiri funkcije, dve osnovni in dve dodatni za kalibracijo in preverjanje senzorja:
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
 * fUNKCIJA aht20_calibrate() pošlje ukaz 0xBE, 0x08, 0x00 za kalibracijo senzorja, če je potrebno.
 * fUNKCIJA aht20_check() pošlje ukaz 0x71 za preverjanje, ali je senzor kalibriran in prisoten. Če ni kalibriran, pokliče funkcijo aht20_calibrate() 
 * za kalibracijo senzorja.
 * 
 * Na koncu kode je dodan še debug izpis surovih podatkov, ki jih senzor vrne, kar lahko pomaga pri razumevanju delovanja senzorja in odpravljanju morebitnih težav.
 * 
 * 
 * @image   components/aht20/images/aht20.png
 * https://files.seeedstudio.com/wiki/Grove-AHT20_I2C_Industrial_Grade_Temperature_and_Humidity_Sensor/AHT20-datasheet-2020-4-16.pdf
*/


#include "aht20.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"




esp_err_t aht20_init(i2c_master_bus_handle_t bus, //   Funkcija za inicializacijo AHT20 senzorja na I2C vodilu 
                     i2c_master_dev_handle_t *dev)
{
    i2c_device_config_t cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x38,
        .scl_speed_hz = 100000,
    };

    return i2c_master_bus_add_device(bus, &cfg, dev);
}


esp_err_t aht20_read(i2c_master_dev_handle_t dev,// Funkcija za branje temperature in vlage s senzorja
                     float *temperature,
                     float *humidity)
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
    *humidity = (h * 100.0f) / 1048576.0f;
    *temperature = (t * 200.0f) / 1048576.0f - 50.0f;


    // Debug: Izpišemo surove podatke
    printf("%02X %02X %02X %02X %02X  %02X %02X\n",
       data[0], data[1], data[2],
       data[3], data[4], data[5], data[6]);


    return ESP_OK;
}

//=======================================================================================================================================

// Dodatne funkcije za kalibracijo in preverjanje senzorja NI NUJNO ZA OSNOVNO DELOVANJE, LAHKO SE JIH DODA KASNEJE ALI PA SE JIH NE DODA, ODVISNO OD POTREB IN ŽELJA UPORABNIKA.

//=========================================================================================================================================

esp_err_t aht20_calibrate(i2c_master_dev_handle_t dev) // Funkcija za kalibracijo senzorja, če je potrebno
{
    uint8_t cmd[] = {0xBE, 0x08, 0x00};
    esp_err_t ret = i2c_master_transmit(dev, cmd, sizeof(cmd), 100);
    if (ret != ESP_OK) return ret;
    vTaskDelay(pdMS_TO_TICKS(10));
    return ESP_OK;
}


esp_err_t aht20_check(i2c_master_dev_handle_t aht) {

uint8_t cmd = 0x71;
uint8_t status;

i2c_master_transmit(aht, &cmd, 1, 100);     // pošlji ukaz 0x71
i2c_master_receive(aht, &status, 1, 100);   // preberi 1 bajt statusa

    if (status & 0x08) {
        printf("Senzor je kalibriran\n");
    } else {
    aht20_calibrate(aht); // Kalibriraj senzor, če ni kalibriran
    printf("Senzor je bil dodatno kalibriran\n");
    }
    return ESP_OK;
}




