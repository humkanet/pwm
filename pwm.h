#ifndef _PWM_H
#define	_PWM_H

#include <stdint.h>


void  pwm_init();
void  pwm_start();
void  pwm_stop();
void  pwm_set(uint16_t pr, uint16_t dc);
void  pwm_set_mode(uint8_t mode);
void  pwm_set_polarity(uint8_t mode);
void  pwm_set_dt(uint8_t rise, uint8_t fall);


#endif
