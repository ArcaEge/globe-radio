#include "wificonnection.h"

extern bool connectToWifi(char* ssid, char* password) {
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_UK)) {
        printf("failed to initialise\n");
        return false;
    }
    cyw43_arch_enable_sta_mode();
    vTaskDelay(250);
    cyw43_wifi_pm(&cyw43_state, 0xa11140);
    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 15000)) {
        printf("failed to connect.\n");
        // Reset
        watchdog_enable(1, 1);
        while (1);
    }
    printf("Connected.\n");

    return true;
}