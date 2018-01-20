#ifndef _UI_H
#define	_UI_H

#include <stdint.h>

#define BUTTON_EVENT_DOWN  0x00
#define BUTTON_EVENT_UP    0x01

typedef void (*BUTTON_EVENT)(uint8_t event);
typedef void (*ENCODER_EVENT)(int8_t inc);

inline void  ui_isr();

inline void  ui_init();
void         ui_button_event(uint8_t btn, BUTTON_EVENT event);
void         ui_encoder_event(uint8_t enc, ENCODER_EVENT event);
void         ui_tick();

#endif
