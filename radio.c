#include "httpclient.h"
#include "wificonnection.h"
#include "picomp3lib/src/mp3dec.h"
#include "hardware/pwm.h"
#include "pico/time.h"
#include <math.h>
#include "hardware/irq.h"
#include "audio.h"

#define SPEAKER 6

uint freq = 1000;
uint amplitude = 255;       // Between 0 and 255 inclusive
uint sampleRate = 11000;
uint samplesPerWave;
//uint pwm_freq = 60000;      // Above audible range
//uint currentWavePos = 0;
//uint maxWavePos;

uint speaker_slice_num;
uint speaker_channel;

float toneValue = 0;
uint64_t cur_pos = 0;
// bool toneUp = true;

bool writeTone() {
    // if (toneUp) {
    //     if (++toneValue >= maxWavePos) {
    //         toneUp = false;
    //     }
    // } else {
    //     if (--toneValue <= 0) {
    //         toneUp = true;
    //     }
    // }
    //toneValue = sin((int)(cur_pos/8));
    //toneValue = sin((cur_pos>>3));
    //if (++cur_pos == 360) cur_pos = 0;
    //uint16_t output = ((toneValue + 1) / 2) * amplitude;
    pwm_set_chan_level(speaker_slice_num, speaker_channel, WAV_DATA[cur_pos] * (amplitude / 255.0f));
    if (++cur_pos >= WAV_DATA_LENGTH) cur_pos = 0;
    //pwm_set_gpio_level(SPEAKER, output);
    // if (cur_pos % 4 == 0)
    // int output = cur_pos % amplitude;
    // pwm_set_chan_level(speaker_slice_num, speaker_channel, cur_pos % amplitude);
    // else pwm_set_chan_level(speaker_slice_num, speaker_channel, 0);
    return true;
}

int main() {
    stdio_init_all();

    gpio_set_function(SPEAKER, GPIO_FUNC_PWM);

    pwm_config cfg = pwm_get_default_config();

    speaker_slice_num = pwm_gpio_to_slice_num(SPEAKER);
    speaker_channel = pwm_gpio_to_channel(SPEAKER);

    samplesPerWave = sampleRate / freq;

    pwm_config_set_wrap(&cfg, 255);
    //pwm_config_set_clkdiv(&cfg, pwmClockDivider);
    //pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_FREE_RUNNING);
    pwm_config_set_phase_correct(&cfg, true);
    pwm_init(speaker_slice_num, &cfg, true);

    pwm_set_chan_level(speaker_slice_num, speaker_channel, 0);

    int time = 1000000UL / sampleRate; // 1,000,000 because number of microseconds in second
    printf("Timer value %d\n", time);
    printf("sampleRate: %dHz\n", sampleRate);
    printf("samplesPerWave: %d\n", samplesPerWave);

    repeating_timer_t timer;
    timer.alarm_id = 0;
    add_repeating_timer_us(-time, writeTone, NULL, &timer);

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

    //sleep_ms(20000);    // 20 seconds

    cancel_repeating_timer(&timer);
    pwm_set_enabled(speaker_slice_num, false);

    while (true) sleep_ms(500);

    cyw43_arch_deinit();
    return 0;
}