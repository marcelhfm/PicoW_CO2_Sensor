# Raspberry Pi Pico W based CO2 Sensor
The Pico W is used to measure the CO2 levels in a room. The value is displayed using a simple LCD display as well as sending the data via MQTT to an external MQTT Broker using the onboard WiFi Chip.

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
