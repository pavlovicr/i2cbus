/**
* \image html shema I2C.jpg "shema I2C" height=200px   
 * @file components/i2c_esp32_master/i2c_esp32_master.c
 * @brief Funkcija i2c_esp32_master_init za konfiguriraacijo in inicializacijo i2c vodila za ESP32 mikrokontroler.
 */


#include "i2c_esp32_master.h"
#include "esp_log.h"


/**
* @param bus_handle, static spremenljivka bo vidna samo znotraj i2c_esp32_master.c. Nobena druga datoteka (npr. main.c ali aht20.c) 
* ne more dostopati do nje neposredno.To je v principu nevarno in slaba praksa, saj lahko vodi do težko odkritih napak, če se slučajno spremeni ali ponastavi. 
* Namesto tega, bodo te datoteke uporabljale funkcijo i2c_esp32_master_get_bus_handle(), ki bo vrnila veljaven ročaj (handle) do I2C vodila, če je bilo uspešno inicializirano.
* @param i2c_esp32_master_init() bo poskrbel za pravilno nastavitev in inicializacijo I2C vodila, 
* vključno z nastavitvijo GPIO pinov, hitrosti komunikacije in drugih parametrov.
* @param i2c_mst_config je struktura, ki vsebuje vse potrebne nastavitve za I2C master vodilo, 
* kot so številka I2C porta, GPIO pini za SDA in SCL, vir ure, število ciklov za ignoriranje motenj in drugo.    
+ @param i2c_new_master_bus esp-idf funkcija, ki se uporablja za ustvarjanje novega I2C master vodila z danimi nastavitvami. 
* Ta funkcija bo poskušala inicializirati I2C vodilo z nastavitvami, ki so določene v strukturi i2c_mst_config,
* in vrnila ročaj (handle) do tega vodila, če je bila inicializacija uspešna. 
* Če pride do napake med inicializacijo, bo funkcija vrnila ustrezno kodo napake, ki jo lahko uporabimo za diagnostiko problema.
*/


static i2c_master_bus_handle_t bus_handle = NULL;
static const char *TAG = "i2c_esp32_master";//<TAG>

esp_err_t i2c_esp32_master_init(void)
{
    if (bus_handle != NULL) {
        ESP_LOGW(TAG, "I2C already initialized");
        return ESP_OK;
    }
    
    i2c_master_bus_config_t i2c_mst_config = {
        .i2c_port = I2C_MASTER_NUM,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
// Inicializacija I2C master vodila z danimi nastavitvami
    esp_err_t ret = i2c_new_master_bus(&i2c_mst_config, &bus_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C init failed: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "I2C master initialized");
    }
    return ret;
}

/**
* @param i2c_esp32_master_get_bus_handle funkcija bo vrnila ročaj (handle) do I2C master vodila, ki je bil inicializiran z i2c_esp32_master_init().
* Ta ročaj se lahko uporablja v drugih delih kode (npr. main.c ali aht20.c) za izvajanje I2C operacij, kot so branje in pisanje podatkov na I2C naprave. 
* Če I2C master vodilo ni bilo uspešno inicializirano, bo funkcija vrnila NULL, kar lahko uporabimo za preverjanje, ali je I2C vodilo pripravljeno za uporabo,
* preden poskušimo izvajati I2C operacije.  
*/

i2c_master_bus_handle_t i2c_esp32_master_get_bus_handle(void)
{
    return bus_handle;
}

/*



esp_err_t i2c_esp32_master_add_device(i2c_master_dev_handle_t *dev_handle, uint16_t dev_addr, uint32_t clk_speed_hz)
{
    if (bus_handle == NULL) {
        ESP_LOGE(TAG, "I2C bus not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = dev_addr,
        .scl_speed_hz = (clk_speed_hz == 0) ? I2C_MASTER_FREQ_HZ : clk_speed_hz,
    };
    return i2c_master_bus_add_device(bus_handle, &dev_cfg, dev_handle);
}

esp_err_t i2c_esp32_master_remove_device(i2c_master_dev_handle_t dev_handle)
{
    return i2c_master_bus_rm_device(dev_handle);
}

esp_err_t i2c_esp32_master_write(i2c_master_dev_handle_t dev_handle, const uint8_t *data, size_t size, int timeout_ms)
{
    return i2c_master_transmit(dev_handle, data, size, timeout_ms);
}

esp_err_t i2c_esp32_master_read(i2c_master_dev_handle_t dev_handle, uint8_t *data, size_t size, int timeout_ms)
{
    return i2c_master_receive(dev_handle, data, size, timeout_ms);
}

esp_err_t i2c_esp32_master_write_read(i2c_master_dev_handle_t dev_handle, const uint8_t *write_data, size_t write_size, uint8_t *read_data, size_t read_size, int timeout_ms)
{
    return i2c_master_transmit_receive(dev_handle, write_data, write_size, read_data, read_size, timeout_ms);
}

*/
