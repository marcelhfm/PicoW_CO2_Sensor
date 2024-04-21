# Raspberry Pi Pico W based CO2 Sensor

Building a CO2 sensor using a Raspberry Pi Pico W, an SSD1306 OLED display and a CO2 Sensor.

## Features

- Measure CO2, humidity and temperature using a SCD40 CO2 sensor
- Display CO2 measurements on the SSD1306 OLED display
- freeRTOS (scheduling of tasks, IPC)
- Measurements are sent to a server via tcp

## Hardware

- Raspberry Pi Pico W
- Sensirion SCD40 CO2 Sensor (SCD41 should work as well, without any code changes). Make sure to power the Sensor with 5V, as I experienced issues when only powering with 3.3V!
- SSD1306 OLED display

### Pins

- Pin 1 = SDA to display
- Pin 2 = SCL to display

- Pin 4 = SDA to sensor
- Pin 5 = SCL to sensor

- Pin 38 = Ground to display and sensor
- Pin 36 = 3V to display

- Sensor needs 5V external power source

## Build

> :warning: I developed this project on a Mac. Some toolings will not work on other machines.

Execute the following steps to build the project:

```
mkdir build && cd build
EXPORT PICO_SDK_PATH=<PATH_TO_THE_PICO_SDK>
EXPORT PICO_BOARD=pico_w
EXPORT WIFI_SSID=<YOUR_SSID>
EXPORT WIFI_SSID=<YOUR_WIFI_PASSWORD>
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
