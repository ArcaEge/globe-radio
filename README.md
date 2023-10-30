# Globe Radio

A work-in-progress Raspberry Pi Pico W based internet radio player

Fixes needed due to bugs in the SDK (as of 30/10/2023):

- SDK needs to be cloned from develop branch, not master (see [#737](https://github.com/raspberrypi/pico-sdk/issues/737))
- ~~In `pico-sdk/src/rp2_common/pico_cyw43_driver/cyw43_driver.c`, the assertions need to be commented out (see [#1526](https://github.com/raspberrypi/pico-sdk/issues/1526))~~ Fixed as of 30/10/2023, waiting for merge into develop branch
