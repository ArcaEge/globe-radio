cmake_minimum_required(VERSION 3.13)

message(STATUS "WIFI_SSID $ENV{WIFI_SSID}")

include(pico_sdk_import.cmake)
set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico_w)

project(radio C CXX ASM)
pico_sdk_init()

add_executable(radio
        radio.c
        audio.h
        wificonnection.c
        httpclient.c
        )

add_subdirectory(picomp3lib/src)

target_include_directories(radio PUBLIC
        ${CMAKE_SOURCE_DIR}/
        )

target_compile_definitions(radio PRIVATE
        WIFI_SSID="$ENV{WIFI_SSID}"
        WIFI_PASSWORD="$ENV{WIFI_PASSWORD}"
        )

target_include_directories(radio PRIVATE
        /home/arca/pico/pico-sdk/**
        )
target_link_libraries(radio
        picomp3lib
        pico_cyw43_arch_lwip_threadsafe_background
        pico_lwip_http
        hardware_timer
        hardware_clocks
        hardware_dma
        hardware_pio
        hardware_pwm
        pico_stdlib
        )

pico_add_extra_outputs(radio)

pico_enable_stdio_usb(radio 1)
pico_enable_stdio_uart(radio 1)
pico_add_extra_outputs(radio)
