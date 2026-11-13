#include "welder.h"
#include "buttons.h"
#include "i2c.h"
#include "ssd1306.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/dwt.h>

static const char *param_name(weld_param_t param)
{
    switch (param) {
    case WELD_PARAM_PULSE1:
        return "P1";
    case WELD_PARAM_PAUSE:
        return "PAUSE";
    case WELD_PARAM_PULSE2:
        return "P2";
    default:
        return "?";
    }
}

static void delay_ms(uint32_t ms)
{
    (void)dwt_enable_cycle_counter();
    uint32_t cycles = ms * 48000U;
    uint32_t start = dwt_read_cycle_counter();
    while ((dwt_read_cycle_counter() - start) < cycles) {
    }
}

int main(void)
{
    rcc_clock_setup_in_hsi_out_48mhz();

    i2c_init();
    ssd1306_init();
    ssd1306_clear();

    welder_init();
    buttons_init();

    uint8_t step = 1;

    while (1) {
        buttons_poll();

        if (button_pressed(BTN_SELECT)) {
            welder_select_next_param();
        }

        if (button_pressed(BTN_MINUS)) {
            welder_adjust_param((int8_t)(-step));
        }
        if (button_pressed(BTN_PLUS)) {
            welder_adjust_param((int8_t)step);
        }
        if (button_pressed(BTN_FIRE)) {
            welder_fire_sequence();
        }

        /* Optional step change is done by holding SELECT? Keep simple: step fixed cycle via MINUS? */
        weld_param_t selected = welder_get_selected_param();

        ssd1306_clear();
        ssd1306_set_cursor(0, 0);
        ssd1306_write_str("P1 ");
        ssd1306_write_uint(welder_get_param(WELD_PARAM_PULSE1));
        ssd1306_write_str("  ");
        ssd1306_write_str("P2 ");
        ssd1306_write_uint(welder_get_param(WELD_PARAM_PULSE2));

        ssd1306_set_cursor(0, 1);
        ssd1306_write_str("PAUSE ");
        ssd1306_write_uint(welder_get_param(WELD_PARAM_PAUSE));
        ssd1306_write_str("  ");
        ssd1306_write_str("EDIT ");
        ssd1306_write_str(param_name(selected));

        ssd1306_set_cursor(0, 2);
        if (welder_break_active()) {
            ssd1306_write_str("BREAK");
        } else if (welder_is_busy()) {
            ssd1306_write_str("FIRING");
        } else {
            ssd1306_write_str("READY");
        }
        ssd1306_write_str("  STEP ");
        ssd1306_write_uint(step);

        delay_ms(50);
    }
}