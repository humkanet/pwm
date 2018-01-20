#ifndef _PWM_H
#define	_PWM_H

#include <stdint.h>


inline void  pwm_init();
void         pwm_start();
void         pwm_stop();
void         pwm_set(uint16_t ton, uint16_t toff);
void         pwm_set_mode(uint8_t mode);


#endif
