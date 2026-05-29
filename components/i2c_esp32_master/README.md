# i2c_esp_master

- vzpostavitev i2c vodila z esp32 masterjem 

<div style="text-align: center;">
  <img src="images/shemai2c.png" width="100">
</div>

https://docs.espressif.com/projects/esp-idf/en/release-v6.0/esp32s3/api-reference/peripherals/i2c.html

## O I2C protokolu

I2C (Inter-Integrated Circuit) je protokol, ki omogoča komunikacijo med napravami preko dveh žic: SDA (Serial Data Line) za prenos podatkov in SCL (Serial Clock Line) za časovno usklajevanje.  
Več na:
- Joplin
- https://www.3dsvet.eu/osnove-komunikacijskega-protokola-i2c/  
- https://www.analog.com/en/resources/technical-articles/i2c-primer-what-is-i2c-part-1.html


## i2c_esp_master - Lasnosti in značilnosti

i2c_esp_master je software za izdelavo i2c vodila z esp32 master napravo.

Konfiguracija vodila **i2c_master_bus_config_t**  zahteva naslednje parametre :

        .i2c_port = I2C_NUM_0,  esp32s3 ima dva porta I2c 0 in 1 
        .sda_io_num = 8, lahko izberemo katerikoli GPIO. Glede na prakso sta 8 in 9 dobra izbira. 
                         V esp32-s3-BOX-3 je sda na GPIO 41, scl na GPIO 40
        .scl_io_num = 9,
        .clk_source = I2C_CLK_SRC_DEFAULT,     clock takt,izberemo defaul, priporočeno. Sicer pa povezan s porabo energije. 
        .glitch_ignore_cnt = 7,           perioda napake vodila, običajna vrednost 7 
        .flags.enable_internal_pullup = true,  omogoča notranji pullup. Priporoča se inštalacija zunanjih pullup uporov, ker zna biti notranji premajhen 

Z **i2c_new_master_bus()** konfiguriramo vodilo, ki bo vrnilo ročaj **bus handle**







