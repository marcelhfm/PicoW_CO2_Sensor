# Raspberry Pi Pico W based CO2 Sensor
The Pico W is used to measure the CO2 levels in a room. The value is displayed using a simple LCD display as well as sending the data via MQTT to an external MQTT Broker using the onboard WiFi Chip.

> :warning: A MacOS device has been used as the dev machine, build and toolings might not work on other machines.

## Functionality 

- Print co2 measurements to SSD1306 OLED display. Connect SDA to GPIO1 and SCL to GPIO2, provide 3.3V power.
- Send co2 measurements via MQTT to external broker / server

## Build 

Execute the following steps to build the project:

```
mkdir build && cd build
EXPORT PICO_SDK_PATH=<PATH_TO_THE_PICO_SDK>
EXPORT PICO_BOARD=pico_w
cmake ..
make

# Optionally if you are using picotool execute this to flash the pico:
picotool load -f blink.uf2
```

## Console Prints via minicom

To see the output of printf on the dev machine install minicom on your device `brew install minicom` and then execute this command: 

```
minicom -b 115200 -o -D /dev/cu.usbmodem1101
```
