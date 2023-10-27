#include "httpclient.h"
#include "wificonnection.h"
#include "picomp3lib/src/mp3dec.h"
#include "hardware/pwm.h"
#include "pico/time.h"
#include <math.h>
#include "audio.h"

#include "audiofile.h"

#define SPEAKER 6

uint amplitude = 255;       // Between 0 and 255 inclusive
uint sampleRate = 11000;

uint speaker_slice_num;
uint speaker_channel;

uint64_t cur_pos = 0;

uint64_t writeTone(struct audioPlayer* player) {
    //pwm_set_gpio_level(player->gpio, WAV_DATA[player->playbackPosition] * (player->amplitude/255.0f) * 4);
    int data = WAV_DATA[player->playbackPosition];
    if (++player->playbackPosition >= WAV_DATA_LENGTH) player->playbackPosition = 0;
    return data;
}

int main() {
    stdio_init_all();

    // Make speaker pin PWM
    gpio_set_function(SPEAKER, GPIO_FUNC_PWM);

    struct audioPlayer player;
    initAudioPlayer(&player, SPEAKER, sampleRate, 8, writeTone);

    playAudio(&player);

    printf("Timer value %d\n", player.sampleTime);
    printf("sampleRate: %dHz\n", sampleRate);

    connectToWifi(WIFI_SSID, WIFI_PASSWORD);

    char *serverName = "de1.api.radio-browser.info";
    char *uri = "/json/stations/topclick/2";

    struct HttpRequest request;

    request.complete = false;

    httpc_state_t* connection = NULL;

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
    httpc_connection_t settings = {
        .use_proxy = 0,
        .headers_done_fn = httpClientHeadersDone,
        .result_fn = httpClientResult
    };

    request.err = httpc_get_file_dns(serverName, HTTP_DEFAULT_PORT, uri, &settings, httpClientReceive, &request, &connection);
    #pragma GCC diagnostic pop

    printf("Waiting for request to complete\n");
    while (!(request.complete)) {
        sleep_ms(10);
    }
    printf("Request complete\n");

    // cancel_repeating_timer(&timer);
    // pwm_set_enabled(speaker_slice_num, false);
    stopAudio(&player);
    sleep_ms(1000);
    playAudio(&player);

    while (true) sleep_ms(500);

    cyw43_arch_deinit();
    return 0;
}