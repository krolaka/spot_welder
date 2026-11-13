#ifndef WELDER_H
#define WELDER_H

#include <stdint.h>
#include <stdbool.h>

#define WELD_PULSE_MIN_MS 1
#define WELD_PULSE_MAX_MS 100
#define WELD_PAUSE_MIN_MS 1
#define WELD_PAUSE_MAX_MS 100

typedef enum {
    WELD_PARAM_PULSE1 = 0,
    WELD_PARAM_PAUSE,
    WELD_PARAM_PULSE2,
    WELD_PARAM_COUNT
} weld_param_t;

void welder_init(void);
bool welder_is_busy(void);
bool welder_break_active(void);
void welder_fire_sequence(void);
void welder_select_next_param(void);
weld_param_t welder_get_selected_param(void);
void welder_adjust_param(int8_t delta);
uint16_t welder_get_param(weld_param_t param);

#endif /* WELDER_H */