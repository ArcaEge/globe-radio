#include "FreeRTOS.h"
#include <task.h>
#include <semphr.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/watchdog.h"

extern bool connectToWifi(char* ssid, char* password);