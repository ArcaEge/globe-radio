#include "audio.h"

void initAudioPlayer(struct audioPlayer *player, uint speakerPin, uint sampleRate, uint bitDepth, uint64_t (*readSample)(struct audioPlayer*)) {
    // Set default settings
    player->isPlaying = false;
    player->playbackPosition = 0;

    player->sampleRate = sampleRate;
    player->sampleTime = 1000000UL / sampleRate;
    player->bitDepth = bitDepth;
    player->bits = pow(2, bitDepth);
    player->amplitude = 255;

    player->gpio = speakerPin;
    player->pwmConfig = pwm_get_default_config();
    player->slice_num = pwm_gpio_to_slice_num(player->gpio);
    player->channel = pwm_gpio_to_channel(player->gpio);

    player->readSampleFunc = readSample;

    // Init PWM
    pwm_config_set_wrap(&player->pwmConfig, 1023);          // 10-bit audio - 2^10 = 1024
    pwm_config_set_phase_correct(&player->pwmConfig, true);
    pwm_init(player->slice_num, &player->pwmConfig, true);

    pwm_set_enabled(player->slice_num, false);

    //pwm_set_chan_level(player->slice_num, player->channel, 0);
}

void playAudio(struct audioPlayer* player) {
    player->isPlaying = true;
    pwm_set_enabled(player->slice_num, true);
    add_repeating_timer_us(-(int)player->sampleTime, timerCallback, player, &player->timer);
}

void pauseAudio(struct audioPlayer* player) {
    player->isPlaying = true;
    cancel_repeating_timer(&player->timer);
    pwm_set_enabled(player->slice_num, false);
}

void stopAudio(struct audioPlayer* player) {
    pauseAudio(player);
    player->playbackPosition = 0;
}

bool timerCallback(repeating_timer_t* rt) {
    struct audioPlayer *player = rt->user_data;
    pwm_set_gpio_level(player->gpio, player->readSampleFunc(player) * (player->amplitude/255.0f) * (1024.0f / player->bits));
    return true;
}
