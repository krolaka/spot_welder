#include "i2c.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>

void i2c_init(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_I2C1);

    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10);
    gpio_set_output_options(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_MED, GPIO9 | GPIO10);
    gpio_set_af(GPIOA, GPIO_AF1, GPIO9 | GPIO10);

    i2c_peripheral_disable(I2C1);
    i2c_set_speed(I2C1, i2c_speed_sm_100k, 48);
    i2c_peripheral_enable(I2C1);
}

void i2c_write(uint8_t addr, const uint8_t *data, uint16_t len)
{
    i2c_transfer7(I2C1, addr, data, len, NULL, 0);
}