#ifndef _MAIN_H
#define	_MAIN_H

#include <stdint.h>
#include "config.h"

#define TICKS_1US  (_XTAL_FREQ/1000000)
#define T_MAX      ((65536/TICKS_1US)-1)
#define T_MIN      1
#define T_ON_MAX   (T_MAX-1)
#define T_OFF_MAX  (T_MAX-1)


#define FREQ_MIN   ((_XTAL_FREQ/65536)+1)
#define FREQ_MAX   (_XTAL_FREQ/25)


#define T_nSTEPS   3
const uint8_t T_STEP[T_nSTEPS] = {
    1,      // x1
    10,     // x10
    100     // x100
};
const char *T_STEP_NAME[T_nSTEPS] = {
    "  1",
    " 10",
    "100"
};


#define FREQ_nSTEPS  6
const uint32_t FREQ_STEP[FREQ_nSTEPS] = {
    1,      // x1
    10,     // x10
    100,    // x100,
    1000,   // x1000
    10000,  // x10000
    100000, // x100000
};
const char *FREQ_STEP_NAME[FREQ_nSTEPS] = {
    "  1",
    " 10",
    "100",
    " 1k",
    "10k",
    ".1M"            
};

#define DUTY_nSTEPS  3
#define DUTY_MIN     1
#define DUTY_MAX     99
const uint8_t DUTY_STEP[DUTY_nSTEPS] = {
    1,
    5,
    10
};
const char *DUTY_STEP_NAME[DUTY_nSTEPS] = {
    " 1",
    " 5",
    "10"
};


#define CTRL_nMODES     2
#define CTRL_MODE_TIME  0x00
#define CTRL_MODE_FREQ  0x01


#define PWM_nMODES      2
const uint8_t PWM_MODE[PWM_nMODES] = {
    0b000,  // Steered PWM
    0b101,  // Push-Pull
};
const char *PWM_MODE_NAME[PWM_nMODES] = {
    "PWM",
    "P-P"
};

#endif
