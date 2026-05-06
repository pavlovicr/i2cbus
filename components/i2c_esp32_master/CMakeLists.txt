idf_component_register(SRCS "i2c_esp32_master.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_driver_i2c freertos esp_timer
)
