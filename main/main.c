/**
 *\image html arlekino-joker.jpg "arlekino" height=200px   
 * @file main.c
 * @brief Main funkcija za inicializacijo in upravljanje z AHT30 senzorjem
 * \image html AHT30.png "AHT30"
 *
 * Ta modul omogoča:
 * - inicializacijo I2C vodila
 * - komunikacijo z AHT30 senzorjem
 * - branje temperature in vlage
 */


#include <stdio.h>
/** @brief driver i2c_master.h */
#include "driver/i2c_master.h"
#include "esp_err.h"

/** @brief GPIO na Box3 SCL master clock */
#define I2C_MASTER_SCL_IO 41          
/** @brief GPIO na Box3 SDA master data */
#define I2C_MASTER_SDA_IO 40          
/** @brief I2C port */
#define I2C_MASTER_NUM I2C_NUM_0  




i2c_master_bus_handle_t bus_handle;

i2c_master_bus_config_t i2c_mst_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = I2C_MASTER_NUM,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
        
    };


i2c_master_dev_handle_t dev_handle;

i2c_device_config_t dev_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = 0x38,
    .scl_speed_hz = 100000,
};




void app_main(void)
{
    
ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
   

ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));





    

}



