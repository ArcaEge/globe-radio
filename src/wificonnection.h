#include "FreeRTOS.h"
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

extern void connectToWifi(void *pvParameters);