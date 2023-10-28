#include "FreeRTOS.h"
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

extern bool connectToWifi(char* ssid, char* password);