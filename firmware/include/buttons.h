#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    BTN_MINUS = 0,
    BTN_PLUS,
    BTN_SELECT,
    BTN_FIRE,
    BTN_COUNT
} button_id_t;

void buttons_init(void);
void buttons_poll(void);
bool button_pressed(button_id_t btn);   /* true once per press */
bool button_held(button_id_t btn);

#endif /* BUTTONS_H */