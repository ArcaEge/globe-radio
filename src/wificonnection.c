#include "wificonnection.h"

extern bool connectToWifi(char* ssid, char* password, SemaphoreHandle_t *printfMutex) {
    if (cyw43_arch_init()) {
        xSemaphoreTake(*printfMutex, portMAX_DELAY);
        printf("failed to initialise\n");
        xSemaphoreGive(*printfMutex);
        return false;
    }
    cyw43_arch_enable_sta_mode();

    xSemaphoreTake(*printfMutex, portMAX_DELAY);
    printf("Connecting to Wi-Fi...\n");
    xSemaphoreGive(*printfMutex);

    while (cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        xSemaphoreTake(*printfMutex, portMAX_DELAY);
        printf("failed to connect. Retrying...\n");
        xSemaphoreGive(*printfMutex);
        vTaskDelay(1000);
    }
    xSemaphoreTake(*printfMutex, portMAX_DELAY);
    printf("Connected\n");
    xSemaphoreGive(*printfMutex);

    return true;
}