#include "FreeRTOS.h"
#include <task.h>
#include "pico/cyw43_arch.h"
//#include <queue.h>
// #include <semphr.h>
#include <stdio.h>
#include "pico/stdlib.h"
//#include "httpclient.h"
//#include "wificonnection.h"
//#include "picomp3lib/src/mp3dec.h"
// #include <math.h>
// #include "lwip/dns.h"

// #include "audiofile.h"
// #include "audio.h"

// #define SPEAKER 6

// const uint sampleRate = 11000;
// ip_addr_t ip;

// static QueueHandle_t xQueue = NULL;
// static SemaphoreHandle_t printfMutex;

// uint64_t writeTone(struct audioPlayer* player) {
//     //pwm_set_gpio_level(player->gpio, WAV_DATA[player->playbackPosition] * (player->amplitude/255.0f) * 4);
//     int data = WAV_DATA[player->playbackPosition];
//     if (++player->playbackPosition >= WAV_DATA_LENGTH) player->playbackPosition = 0;
//     return data;
// }

// void audioTask(void *pvParameters) {
//     // Make speaker pin PWM
//     gpio_set_function(SPEAKER, GPIO_FUNC_PWM);

//     struct audioPlayer player;
//     initAudioPlayer(&player, SPEAKER, sampleRate, 8, writeTone);

//     playAudio(&player);

//     xSemaphoreTake(printfMutex, portMAX_DELAY);
//     printf("Timer value %d\n", player.sampleTime);
//     printf("sampleRate: %dHz\n", sampleRate);
//     xSemaphoreGive(printfMutex);

//     while(true) {
//         vTaskDelay(500);
//     }
// }

void wifiConnectTask(void *pvParameters) {
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return;
    }
    cyw43_arch_enable_sta_mode();
    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect.\n");
        exit(1);
    } else {
        printf("Connected.\n");
    }
    while (true) {
        vTaskDelay(500);
    }
}

int main() {
    stdio_init_all();

    // printfMutex = xSemaphoreCreateMutex();

    TaskHandle_t wifiHandle;

    //xTaskCreate(audioTask, "Audio", 1024, NULL, 1, NULL);
    xTaskCreate(wifiConnectTask, "wifiConnect", configMINIMAL_STACK_SIZE, NULL, 1, &wifiHandle);

    // Note: tried with and without this line, no difference
    vTaskCoreAffinitySet(wifiHandle, 1);
    
    vTaskStartScheduler();

    while (true)
        vTaskDelay(500);

    cyw43_arch_deinit();
    return 0;
    //connectToWifi(WIFI_SSID, WIFI_PASSWORD);

    // char *serverName = "de1.api.radio-browser.info";
    // char *uri = "/json/stations/topclick/2";

    // struct HttpRequest request;

    // request.complete = false;

    // httpc_state_t* connection = NULL;

    // #pragma GCC diagnostic push
    // #pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
    // httpc_connection_t settings = {
    //     .use_proxy = 0,
    //     .headers_done_fn = httpClientHeadersDone,
    //     .result_fn = httpClientResult
    // };

    // request.err = httpc_get_file_dns(serverName, HTTP_DEFAULT_PORT, uri, &settings, httpClientReceive, &request, &connection);
    // #pragma GCC diagnostic pop

    // printf("Waiting for request to complete\n");
    // while (!(request.complete)) {
    //     sleep_ms(10);
    // }
    // printf("Request complete\n");

    // // cancel_repeating_timer(&timer);
    // // pwm_set_enabled(speaker_slice_num, false);
    // stopAudio(&player);
    // sleep_ms(1000);
    // playAudio(&player);

}