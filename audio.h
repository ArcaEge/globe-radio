#include "hardware/pwm.h"
#include "pico/time.h"
#include <math.h>

struct audioPlayer {
    bool isPlaying;
    uint64_t playbackPosition;                          // Position of playback, getSample function has control over incrementing this value, not the audio player.

    uint sampleRate;                                    // Samples per second. Typically 11025, 22050 or 44100
    uint sampleTime;                                    // Equivalent to 1,000,000 / sample rate. 1,000,000 because number of microseconds in second. Auto-populated
    uint bitDepth;                                      // Bits per sample - used to scale audio to 12 bit
    uint64_t bits;
    uint amplitude;                                     // Between 0 and 255 inclusive
    repeating_timer_t timer;

    uint gpio;                                          // Speaker pin
    pwm_config pwmConfig;                               // PWM config
    uint slice_num;                                     // Auto-populated
    uint channel;                                       // Auto-populated

    uint64_t (*readSampleFunc)(struct audioPlayer*);    // Audio player instance passed as a pointer into function
};

// Initialise audioPlayer values
void initAudioPlayer(struct audioPlayer *player, uint speakerPin, uint sampleRate, uint bitDepth, uint64_t (*getSample)(struct audioPlayer*));

void playAudio(struct audioPlayer *player);
void pauseAudio(struct audioPlayer *player);
void stopAudio(struct audioPlayer *player);

bool timerCallback(repeating_timer_t *rt);