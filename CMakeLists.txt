cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico_w)

#set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# Import those libraries
include(pico_sdk_import.cmake)

# Define project
project(radio C CXX ASM)
#enable_language( C CXX ASM )

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Set any variables required for importing libraries
SET(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/FreeRTOS-Kernel)

include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

add_library(freertos_config INTERFACE)
target_include_directories(freertos_config
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Config
)

# add_compile_options(
#     -mcpu=cortex-m0plus
#     # -mthumb
#     # -mfpu=fpv5-sp-d16
#     # -mfloat-abi=hard

#     # -fdata-sections
#     # -ffunction-sections
#     # -fno-strict-aliasing

#     # -Wall

#     # $<$<CONFIG:Debug>:-Og>
# )

# add_library(FreeRTOS STATIC
#     lib/FreeRTOS-Kernel/event_groups.c
#     lib/FreeRTOS-Kernel/list.c
#     lib/FreeRTOS-Kernel/queue.c
#     lib/FreeRTOS-Kernel/stream_buffer.c
#     lib/FreeRTOS-Kernel/tasks.c
#     lib/FreeRTOS-Kernel/timers.c
#     lib/FreeRTOS-Kernel/portable/MemMang/heap_3.c
#     lib/FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c
# )

# target_include_directories(FreeRTOS PUBLIC
#     Config/
#     lib/FreeRTOS-Kernel/include
#     lib/FreeRTOS-Kernel/portable/GCC/ARM_CM0
# )


# Add subdirectories
add_subdirectory(src)
add_subdirectory(lib)