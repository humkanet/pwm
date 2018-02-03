#ifndef _OPTIONS_H
#define _OPTIONS_H

#include <stdint.h>

#define TICKS_1US        (_XTAL_FREQ/1000000)


/* ���������� �������� */
#define POLARITY_nMODES  4
extern const char*       POLARITY_NAMES[POLARITY_nMODES];


/* ������ ����������� */
#define CTRL_nMODES      3
#define CTRL_MODE_TIME   0x00
#define CTRL_MODE_FREQ   0x01
#define CTRL_MODE_DT     0x02
extern const char*       CTRL_MODES_LINE1[CTRL_nMODES];
extern const char*       CTRL_MODES_LINE2[CTRL_nMODES];


/* ����������� Ton+Toff */
#define T_MAX            ((65536/TICKS_1US)-1)
#define T_MIN            1
#define T_ON_MAX         (T_MAX-1)
#define T_OFF_MAX        (T_MAX-1)


/* ����������� Freq+Duty */
#define FREQ_MIN         ((_XTAL_FREQ/65536)+1)
#define FREQ_MAX         (_XTAL_FREQ/25)
#define DUTY_MIN         1
#define DUTY_MAX         99


/* ��� ��������� ������� */
#define FREQ_nSTEPS      6
extern const uint16_t    FREQ_STEP[FREQ_nSTEPS];
extern const char*       FREQ_STEP_NAMES[FREQ_nSTEPS];


/* ��� ��������� ������� */
#define T_nSTEPS         3
extern const uint8_t     T_STEP[T_nSTEPS];
extern const char*       T_STEP_NAMES[T_nSTEPS];


/* ��� ��������� ���������� */
#define DUTY_nSTEPS      3
extern const uint8_t     DUTY_STEP[DUTY_nSTEPS];
extern const char*       DUTY_STEP_NAMES[DUTY_nSTEPS];


/* ������ ��������� �������� */
#define PWM_nMODES       3
extern const uint8_t     PWM_MODE[PWM_nMODES];
extern const char*       PWM_MODE_NAMES[PWM_nMODES];


/* ����������� DEADTIME */
#define DT_MIN           0
#define DT_MAX           63


typedef struct {
	uint8_t   ctrl_mode;    // ����� ����������� ���������� (Freq+Duty, Ton+Toff, DT)
	uint8_t   pwm_mode;     // ����� ������ ���
	uint8_t   polarity;     // ���������� ��������
	uint16_t  pr;           // ������� PR
	uint16_t  dc;           // ������� DC
	uint16_t  ton;          // Ton (���)
	uint16_t  toff;         // Toff (���)
	uint8_t   dtr;          // DEADTIME (rise)
	uint8_t   dtf;          // DEADTIME (fall)
	uint8_t   freq_step;    // ��� ��������� �������
	uint8_t   duty_step;    // ��� ��������� �������
	uint8_t   ton_step;     // ��� ��������� Ton
	uint8_t   toff_step;    // ��� ��������� Toff
} OPTIONS;
extern OPTIONS opt;


uint16_t  us2pr(uint16_t us);
uint16_t  pr2us(uint16_t pr);
uint16_t  freq2pr(uint32_t freq);
uint32_t  pr2freq(uint16_t pr);

#endif
