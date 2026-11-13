#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

#define SSD1306_ADDR 0x3C

void ssd1306_init(void);
void ssd1306_clear(void);
void ssd1306_set_cursor(uint8_t x, uint8_t page);
void ssd1306_write_char(char c);
void ssd1306_write_str(const char *s);
void ssd1306_write_uint(uint16_t value);

#endif /* SSD1306_H */