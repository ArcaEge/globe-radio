#include "wificonnection.h"

extern bool connectToWifi(char* ssid, char* password) {
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return false;
    }
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");

    while (cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect. Retrying...\n");
        vTaskDelay(2500);
    }
    printf("Connected\n");

    return true;
}