#include "welder.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/dwt.h>

static volatile uint16_t pulse1_ms = 5;
static volatile uint16_t pause_ms = 10;
static volatile uint16_t pulse2_ms = 20;
static volatile weld_param_t selected_param = WELD_PARAM_PULSE1;
static volatile bool sequence_busy = false;

static void delay_ms(uint32_t ms)
{
    (void)dwt_enable_cycle_counter();
    uint32_t cycles = ms * 48000U;
    uint32_t start = dwt_read_cycle_counter();
    while ((dwt_read_cycle_counter() - start) < cycles) {
    }
}

static void fire_one_pulse(uint16_t duration_ms)
{
    if (duration_ms < WELD_PULSE_MIN_MS) {
        duration_ms = WELD_PULSE_MIN_MS;
    }
    if (duration_ms > WELD_PULSE_MAX_MS) {
        duration_ms = WELD_PULSE_MAX_MS;
    }

    timer_set_period(TIM1, duration_ms - 1);
    timer_set_oc_value(TIM1, TIM_OC3, duration_ms - 1);
    TIM1_CNT = 0;
    TIM1_EGR = TIM_EGR_UG;
    TIM1_CR1 |= TIM_CR1_CEN;

    while ((TIM1_CR1 & TIM_CR1_CEN) != 0) {
        if (welder_break_active()) {
            TIM1_CR1 &= ~TIM_CR1_CEN;
            break;
        }
    }
}

void welder_init(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_TIM1);

    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6);
    gpio_set_af(GPIOA, GPIO_AF2, GPIO6);

    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO1);
    gpio_set_af(GPIOB, GPIO_AF2, GPIO1);

    timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_set_prescaler(TIM1, 48000 - 1);
    timer_set_period(TIM1, 5 - 1);
    timer_set_oc_mode(TIM1, TIM_OC3, TIM_OCM_PWM1);
    timer_set_oc_value(TIM1, TIM_OC3, 5 - 1);
    TIM1_CCER |= TIM_CCER_CC3NE;
    TIM1_CCER &= ~(TIM_CCER_CC3P | TIM_CCER_CC3NP);
    TIM1_BDTR = TIM_BDTR_MOE | TIM_BDTR_BKE | TIM_BDTR_OSSR;
    TIM1_CR1 |= TIM_CR1_OPM;
}

bool welder_is_busy(void)
{
    return sequence_busy || ((TIM1_CR1 & TIM_CR1_CEN) != 0);
}

bool welder_break_active(void)
{
    return !gpio_get(GPIOA, GPIO6);
}

void welder_fire_sequence(void)
{
    if (welder_is_busy()) {
        return;
    }
    if (welder_break_active()) {
        return;
    }

    sequence_busy = true;

    fire_one_pulse(pulse1_ms);
    if (!welder_break_active()) {
        delay_ms(pause_ms);
    }
    if (!welder_break_active()) {
        fire_one_pulse(pulse2_ms);
    }

    sequence_busy = false;
}

void welder_select_next_param(void)
{
    selected_param = (weld_param_t)((selected_param + 1) % WELD_PARAM_COUNT);
}

weld_param_t welder_get_selected_param(void)
{
    return selected_param;
}

void welder_adjust_param(int8_t delta)
{
    switch (selected_param) {
    case WELD_PARAM_PULSE1:
        {
            int32_t value = (int32_t)pulse1_ms + delta;
            if (value < WELD_PULSE_MIN_MS) value = WELD_PULSE_MIN_MS;
            if (value > WELD_PULSE_MAX_MS) value = WELD_PULSE_MAX_MS;
            pulse1_ms = (uint16_t)value;
        }
        break;
    case WELD_PARAM_PAUSE:
        {
            int32_t value = (int32_t)pause_ms + delta;
            if (value < WELD_PAUSE_MIN_MS) value = WELD_PAUSE_MIN_MS;
            if (value > WELD_PAUSE_MAX_MS) value = WELD_PAUSE_MAX_MS;
            pause_ms = (uint16_t)value;
        }
        break;
    case WELD_PARAM_PULSE2:
        {
            int32_t value = (int32_t)pulse2_ms + delta;
            if (value < WELD_PULSE_MIN_MS) value = WELD_PULSE_MIN_MS;
            if (value > WELD_PULSE_MAX_MS) value = WELD_PULSE_MAX_MS;
            pulse2_ms = (uint16_t)value;
        }
        break;
    default:
        break;
    }
}

uint16_t welder_get_param(weld_param_t param)
{
    switch (param) {
    case WELD_PARAM_PULSE1:
        return pulse1_ms;
    case WELD_PARAM_PAUSE:
        return pause_ms;
    case WELD_PARAM_PULSE2:
        return pulse2_ms;
    default:
        return 0;
    }
}