cmake_minimum_required(VERSION 3.28)

set(PROJECT_NAME "picow_co2_sensor")
set(BUILD_NUMBER "1")

set(APP_NAME "co2")
set(APP_VERSION_NUMBER "0.0.2")

set(APP_SRC_DIRECTORY "${CMAKE_SOURCE_DIR}/app")

add_compile_definitions($<$<CONFIG:Debug>:DEBUG=1>)

#set(ENV{PICO_SDK_PATH} "${CMAKE_SOURCE_DIR/../pico/pico-sdk}")
#set(ENV{PICO_BOARD} "pico_w")

set(FREERTOS_KERNEL_PATH "${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel")

SET(FREERTOS_CONFIG_FILE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/config/FreeRTOS-Kernel" CACHE STRING "Local Config")

set(CORE_MQTT_PATH "${CMAKE_SOURCE_DIR}/coreMQTT")
set(COREMQTT_PORT_PATH ${CMAKE_CURRENT_LIST_DIR}/config/CoreMQTT) # coreMQTT config files

set(COREMQTT_AGENT_PORT_PATH ${CMAKE_CURRENT_LIST_DIR}/config/CoreMQTT-Agent) # coreMQTT config files
set(CORE_MQTT_AGENT_PATH "${CMAKE_SOURCE_DIR}/coreMQTT-Agent")

include(lwip_import.cmake) # lwip config files
include_directories("${FREERTOS_CONFIG_FILE_DIRECTORY}") # FreeRTOS config files

include(pico_sdk_import.cmake)

project(${PROJECT_NAME}
    LANGUAGES C CXX ASM
    DESCRIPTION "PicoW CO2 Sensor"   
    )

  pico_sdk_init()

# Initialize all other external libs & dependencies
include(FreeRTOS_Kernel_import.cmake) # Configure FreeRTOS Kernel
