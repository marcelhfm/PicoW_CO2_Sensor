cmake_minimum_required(VERSION 3.28)

project(co2 C CXX ASM)
include(pico_sdk_import.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(co2
  picow_blink.c
  ssd1306.c
)

# Uncomment if you want to flash your board via picotool.
pico_add_extra_outputs(co2)
pico_enable_stdio_usb(co2 1)
pico_enable_stdio_uart(co2 0)

target_link_libraries(co2
  pico_stdlib
  # Uncomment when using Pico W
  pico_cyw43_arch_none
  hardware_i2c
)


