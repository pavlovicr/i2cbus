idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES i2c_esp32_master
                             aht20)
