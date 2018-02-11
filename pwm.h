#ifndef _PWM_H
#define	_PWM_H

#include <stdint.h>

#define RISE_TIME_NS  70
#define FALL_TIME_NS  30

#define PR2FREQ(x)    (_XTAL_FREQ/(x+1))
#define FREQ2PR(x)    ((_XTAL_FREQ/x)-1)

#define DC_MIN        (((RISE_TIME_NS+FALL_TIME_NS)/(1000000000/_XTAL_FREQ))+1)
#define FREQ_MAX      (_XTAL_FREQ/(50*DC_MIN))
#define FREQ_MIN      (PR2FREQ(65535)+1)
#define PR_MIN        FREQ2PR(FREQ_MAX)
#define PR_MAX        FREQ2PR(FREQ_MIN)



void  pwm_init();
void  pwm_start();
void  pwm_stop();
void  pwm_set(uint16_t pr, uint16_t dc);
void  pwm_set_mode(uint8_t mode);
void  pwm_set_polarity(uint8_t mode);
void  pwm_set_dt(uint8_t rise, uint8_t fall);


#endif
