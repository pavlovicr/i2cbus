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












P

## Potek

I2C timing diagram (Branje)
Tipično 2 baytno branje z lokacije prednastavljenega pointerja,  kot so temperatura, temperatura visoka in temperatura nizka.  

<div style="text-align: center;">
        <img src="images/LM75_diagram_branje.png" width="100">
</div>

Potek (brez nastavljanja pointerja):

Master pošlje START
- Pošlje naslov naprave + READ bit

LM75 vrne:
- MSB (prvi byte temperature)
- LSB (drugi byte temperature)

Master pošlje NACK in STOP



Kdaj pointer NI preset?

Če si prej dostopal do drugega registra (npr. THIGH), potem:

pointer kaže tja ❗
če hočeš brati temperaturo, moraš narediti:
START
naslov + WRITE
poslati 0x00 (TEMP register)
REPEATED START
naslov + READ
branje 2 bajtov


Ko master pošlje naslov naprave, pošlje skupaj 8 bitov:

[ 7-bitni naslov ] + [ R/W bit ]
R/W = 0 → WRITE
R/W = 1 → READ

<div style="text-align: center;">
        <img src="images/LM75_diagram_write.png" width="100">
</div>




## Hardware Requirements

- ESP32-BOX-3 development board
- AHT21 temperature and humidity sensor connected to:
  - SCL: GPIO40
  - SDA: GPIO41


Povzetek :

- I2C_MASTER_SCL_IO           40      
- I2C_MASTER_SDA_IO           41  
- I2C_MASTER_FREQ_HZ          100000  
- I2C_MASTER_TIMEOUT_MS       1000
- AHT30_I2C_ADDR              0x38 
- AHT30_CMD_INIT              0xBE
- AHT30_CMD_TRIGGER           0xAC 
- AHT30_CMD_SOFT_RESET        0xBA 
- AHT30_MEASUREMENT_DELAY_MS  80  








## Software Requirements

 

## Building and Flashing

1. Set up ESP-IDF environment
2. Navigate to the project directory
3. Configure the project: `idf.py menuconfig`
4. Build the project: `idf.py build`
5. Flash to device: `idf.py flash`
6. Monitor output: `idf.py monitor`

## Usage

The application initializes the display and sensor, then continuously reads and displays temperature and humidity every 2 seconds. If sensor initialization fails, it displays an error message on the screen.

## Components

- `display_manager`: Handles LCD display initialization and brightness control
- `sensor_manager`: Manages AHT21 sensor communication and data reading

## License

[Add license information here]# TermostatBox3
