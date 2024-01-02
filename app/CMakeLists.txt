cmake_minimum_required(VERSION 3.28)

add_compile_definitions(APP_NAME="${APP_NAME}")
add_compile_definitions(APP_VERSION="${APP_VERSION}")
add_compile_definitions(BUILD_NUM=${BUILD_NUM})

add_executable(${APP_NAME}
  main.c
  ssd1306/ssd1306.c
  ssd1306/framebuffer.c
  ssd1306/display.c
  tasks/update_display_task.c
  tasks/read_data_task.c
)

target_link_libraries(${APP_NAME} LINK_PUBLIC
  pico_stdlib
  # Uncomment when using Pico W
  pico_cyw43_arch_none
  hardware_i2c
  hardware_adc
  FreeRTOS
)

# Uncomment if you want to flash your board via picotool.
pico_add_extra_outputs(co2)
pico_enable_stdio_usb(co2 1)
pico_enable_stdio_uart(co2 0)


