#include "buttons.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

static const uint16_t btn_pins[BTN_COUNT] = {
    GPIO0, GPIO1, GPIO2, GPIO3
};

static bool last_raw[BTN_COUNT];
static uint8_t debounce[BTN_COUNT];
static bool stable[BTN_COUNT];
static bool pressed_flag[BTN_COUNT];
static bool held_state[BTN_COUNT];

void buttons_init(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP,
                   GPIO0 | GPIO1 | GPIO2 | GPIO3);

    for (int i = 0; i < BTN_COUNT; i++) {
        last_raw[i] = true;
        debounce[i] = 0;
        stable[i] = false;
        pressed_flag[i] = false;
        held_state[i] = false;
    }
}

void buttons_poll(void)
{
    for (int i = 0; i < BTN_COUNT; i++) {
        bool raw_pressed = !gpio_get(GPIOA, btn_pins[i]);

        if (raw_pressed == last_raw[i]) {
            if (debounce[i] < 3) {
                debounce[i]++;
            }
            if (debounce[i] >= 3) {
                if (stable[i] != raw_pressed) {
                    stable[i] = raw_pressed;
                    if (raw_pressed) {
                        pressed_flag[i] = true;
                    }
                }
                held_state[i] = stable[i];
            }
        } else {
            debounce[i] = 0;
        }

        last_raw[i] = raw_pressed;
    }
}

bool button_pressed(button_id_t btn)
{
    if (btn >= BTN_COUNT) {
        return false;
    }
    bool value = pressed_flag[btn];
    pressed_flag[btn] = false;
    return value;
}

bool button_held(button_id_t btn)
{
    if (btn >= BTN_COUNT) {
        return false;
    }
    return held_state[btn];
}