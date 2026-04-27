# AHT30 senzor temperature 

Na I2C vodilo priključimo senzor AHT30 za merjenje vlage in temperature. 
V konkretnem primeru je senzor vgrajen v ESPS3-BOX-3 in priključen na GPIO 40 in 41.

<div style="text-align: center;">
  <img src="images/SENSOR.jpg" width="100">
  <img src="images/ESP32-S3-BOX-3-SENSOR.png" width="100">
  <img src="images/AHT30.png" width="100">
</div>

## O I2C protokolu
https://www.3dsvet.eu/osnove-komunikacijskega-protokola-i2c/  

I2C (Inter-Integrated Circuit) je protokol, ki omogoča komunikacijo med napravami preko dveh žic: SDA (Serial Data Line) za prenos podatkov in SCL (Serial Clock Line) za časovno usklajevanje. Ta protokol vključuje tudi povezavo do mase in VCC za napajanje. Uporovniki, običajno med 2.2kΩ do 10kΩ, ohranjajo SDA in SCL v visokem stanju, ko so v mirovanju. Vsaka naprava v mreži potrebuje edinstven naslov, kar omogoča komunikacijo med več napravami z minimalno ožičenjem.

<div style="text-align: center;">
  <img src="images/shema I2C.png" width="300">
</div>


KAKO DELUJE I2C?
Vodilo SDA in SCL sta dve žici vezani prek uporov na tok Vcc ( 3.3V ali 5V)

Vcc vzdržuje na vodilu SDA stanje HIGH. (pasivno stanje). 
I2C naprave master in slave uporabljajo open-drain (NMOS ,open collector) s katerim potegnejo tok proti GND.
Tako vzpostavijo na SDA vodilu stanje LOW 

Na SDA se stanje spreminja v odvisnosti od master ali slave. Če eden ali drugi potegne linijo na maso nastane LOW.
SDA je podatkovno vodilo. HIGH pomeni 1, LOW pomeni 0. 
V stanju mirovanja je na vodilu SDA stalno HIGH torej 1.
Ko kateri od masterjev ali slave odpre open-drain in potegne tok proti GND pa nastane napetostno stanje LOW torej 0. 

Na SCL se stanji HIGH in LOW spreminja s taktom ure.

Kako se ustvarjajo biti.
V vsakem taktu ure , v stanju HIGH na SCL vodilu  ali na koncu stanja HIGH se odčitavajo informacije na SDA vodilu. To so biti , ki jih zapisujeta in bereta master in slave. 

Vse slave naprave berejo informacije o naslovu , ki jih pošilja master. Vsak slave mora imeti :

- shift register , ki je 8 bitni serijski vmesnik , bolj točno 8-bitni serial/parallel shift register znotraj I²C hardverskega modula.

Kaj shift register dejansko dela: 

Pri sprejemu (RX):
vsak takt SCL:
1 bit pride iz SDA
se “potisne” v register

Po 8 taktih:

imaš cel byte

👉 to je serijski → paralelni pretvornik (SIPO)

Pri oddajanju (TX):
register ima pripravljen byte
vsak takt SCL:
izpiše en bit na SDA

👉 to je paralelni → serijski pretvornik (PISO)


Vedno je aktiven samo en "govorec". Ostali poslušajo in čakajo na poziv, na sprejem njihovega naslova. 



- address comparator, da ugotovi ali je data za njega 
- ACK/NACK logiko
- control state machine




<div style="text-align: center;">
  <img src="images/i2c hardware.png" width="300">
</div>

Protokol podrobno : https://www.analog.com/en/resources/technical-articles/i2c-primer-what-is-i2c-part-1.html


### ESP32-S3-SENSOR-01_V1.1 senzor AHT30  Specifikacije  vezane na kodo

<div style="text-align: center;">
  <img src="images/ESP32S3-BOX-3 - AHT30.png" width="200">
</div>

https://eleparts.co.kr/data/goods_attach/202306/good-pdf-12751003-1.pdf


KAJ PA TO !!!!!!!!!!!!!!!!!!!!!
https://components.espressif.com/components/espressif/aht30/versions/1.0.0/readme




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










## Features

- Real-time temperature and humidity display
- AHT21 sensor integration via I2C
- LVGL-based user interface
- ESP32-BOX-3 hardware support

## Hardware Requirements

- ESP32-BOX-3 development board
- AHT21 temperature and humidity sensor connected to:
  - SCL: GPIO40
  - SDA: GPIO41

## Software Requirements

- ESP-IDF framework
- LVGL graphics library
- ESP-BSP (Board Support Package)

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
