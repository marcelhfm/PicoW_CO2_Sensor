# Raspberry Pi Pico W based CO2 Sensor

Building a CO2 sensor using a Raspberry Pi Pico W, an SSD1306 OLED display and a CO2 Sensor.

## Features 

- Display CO2 measurements on the OLED display 
- Measurements are also send to an external MQTT broker

## Build 

> :warning: I developed this project on a Mac. Some toolings will not work on other machines.

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

## Contribute

Feel free to contribute, open issues, ask questions or fork this project! :) 
