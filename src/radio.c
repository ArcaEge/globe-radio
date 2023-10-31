#include <FreeRTOS.h>
#include <task.h>
#include "pico/cyw43_arch.h"
//#include <queue.h>
#include <semphr.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "httpClient.h"
#include "wifiConnection.h"
#include "stream.h"
#include "ipUtils.h"
#include "hardware/exception.h"

#include "audioFile.h"
#include "audio.h"

#define SPEAKER 6

#define log_core_num(fmt, ...) printf("[core %d] " fmt, portGET_CORE_ID(), ##__VA_ARGS__)

const uint sampleRate = 11000;
// ip_addr_t ip;

// static QueueHandle_t xQueue = NULL;
static SemaphoreHandle_t printfMutex;

static void waitForDebugger() {
    log_core_num("Waiting for debugger...\n");
    __breakpoint();
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
    log_core_num("Stack overflow in task '%s'\n", pcTaskName);
    waitForDebugger();
}

static void hardfault_handler() {
    log_core_num("hardfault!!!\n");
    waitForDebugger();
}

uint64_t writeTone(struct audioPlayer* player) {
    //pwm_set_gpio_level(player->gpio, WAV_DATA[player->playbackPosition] * (player->amplitude/255.0f) * 4);
    int data = WAV_DATA[player->playbackPosition];
    if (++player->playbackPosition >= WAV_DATA_LENGTH) player->playbackPosition = 0;
    return data;
}

void audioTask(void* pvParameters) {
    // Make speaker pin PWM
    gpio_set_function(SPEAKER, GPIO_FUNC_PWM);

    struct audioPlayer player;
    initAudioPlayer(&player, SPEAKER, sampleRate, 8, writeTone);

    playAudio(&player);

    xSemaphoreTake(printfMutex, portMAX_DELAY);
    printf("Timer value %d\n", player.sampleTime);
    printf("sampleRate: %dHz\n", sampleRate);
    xSemaphoreGive(printfMutex);

    while (true) {
        vTaskDelay(500);
    }
}

void webRequestTask(void* pvParameters) {
    char* serverName = "de1.api.radio-browser.info";
    char* uri = "/json/stations/topclick/2";

    struct HttpRequest request;
    request.complete = false;

    httpc_state_t* connection = NULL;

    ip4_addr_t hostAddress = getAddressFromHost(serverName);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
    httpc_connection_t settings = getConnectionSettings();
    request.err = httpc_get_file(&hostAddress, HTTP_DEFAULT_PORT, uri, &settings, httpClientReceive, &request, &connection);
#pragma GCC diagnostic pop

    printf("Waiting for request to complete\n");
    while (!(request.complete)) {
        sleep_ms(10);
    }
    printf("Request complete\n");

    while (true) {
        vTaskDelay(500);
    }
}

void wifiConnectTask(void* pvParameters) {
    connectToWifi(WIFI_SSID, WIFI_PASSWORD);

    xTaskCreate(webRequestTask, "webRequest", 2048, NULL, 1, NULL);
    xTaskCreate(audioTask, "Audio", 1024, NULL, 1, NULL);

    vTaskDelete(NULL);
}

int main() {
    stdio_init_all();
    exception_set_exclusive_handler(HARDFAULT_EXCEPTION, hardfault_handler);

    printfMutex = xSemaphoreCreateMutex();

    TaskHandle_t wifiHandle;
    xTaskCreate(wifiConnectTask, "wifiConnect", 512, NULL, 2, &wifiHandle);

    //vTaskCoreAffinitySet(wifiHandle, (1 << 0));     // Core 0

    vTaskStartScheduler();

    return 0;
}