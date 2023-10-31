cmake_minimum_required(VERSION 3.13)

add_executable(radio
        radio.c
        audio.c
        stream.c
        ipUtils.c
        audioFile.h
        wifiConnection.c
        httpClient.c
        )

target_include_directories(radio PRIVATE
        ${CMAKE_SOURCE_DIR}/
        ${CMAKE_SOURCE_DIR}/src
        Config/
        lib/FreeRTOS-Kernel/include
        lib/FreeRTOS-Kernel/portable/GCC/ARM_CM0
        /home/arca/pico/pico-sdk/**
)

target_compile_definitions(radio PRIVATE
        WIFI_SSID="$ENV{WIFI_SSID}"
        WIFI_PASSWORD="$ENV{WIFI_PASSWORD}"
        )

add_compile_definitions(
        FREE_RTOS_KERNEL_SMP=1
        portSUPPORT_SMP=1
)

target_link_libraries(radio
        freertos_config
        pico_stdlib
        pico_multicore
        pico_cyw43_arch_lwip_sys_freertos
        pico_lwip_http
        hardware_timer
        hardware_exception
        hardware_clocks
        hardware_pwm
        FreeRTOS-Kernel-Heap4
        )

pico_enable_stdio_usb(radio 1)
pico_enable_stdio_uart(radio 1)

pico_add_extra_outputs(radio)
