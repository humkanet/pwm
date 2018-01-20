#include <xc.h>
#include "pre.h"
#include "pwm.h"

#define PWM_PORTA   C
#define PWM_PINA    5
#define PWM_PORTB   C
#define PWM_PINB    4

#define U16(x)      ((uint16_t) (x))

struct {
	union {
		struct {
			uint8_t  active : 1;
		};
		uint8_t      flags;
	};
} pwm;


inline void pwm_init()
{
	pwm.active = 0;
	// Настраиваем пины
	LAT(PWM_PORTA, PWM_PINA)  = 0;
	LAT(PWM_PORTB, PWM_PINB)  = 0;
	TRIS(PWM_PORTA, PWM_PINA) = 0;
	TRIS(PWM_PORTB, PWM_PINB) = 0;
	// Разрешаем 100ма на пинах
	HIDC4      = 1;
	HIDC5      = 1;
	// Настраиваем PWM5
	PWM5CON    = 0x00;
	PWM5CLKCON = 0x00;
	PWM5INTE   = 0x00;
	PWM5INTF   = 0x00;
	PWM5LDCON  = 0x00;
	// Подключаем пины к COG
	PPS(PWM_PORTA, PWM_PINA) = 0x04;
	PPS(PWM_PORTB, PWM_PINB) = 0x05;
	// Настраиваем COG
	COG1CON0   = 0x00;
	COG1CON1   = 0x00;
	COG1DBF    = 0x00;
	COG1DBR    = 0x00;
	COG1BLKF   = 0x00;
	COG1BLKR   = 0x00;
	COG1PHF    = 0x00;
	COG1PHR    = 0x00;
	COG1ASD0   = 0x14;
	COG1ASD1   = 0x00;
	COG1RIS0   = 0x00;
	COG1RIS1   = 0x02;
	COG1RSIM0  = 0x00;
	COG1RSIM1  = 0x00;
	COG1FIS0   = 0x00;
	COG1FIS1   = 0x02;
	COG1FSIM0  = 0x00;
	COG1FSIM1  = 0x00;
	COG1STR    = 0x03;
	COG1CON0   = 0b10001000;
}


void pwm_start()
{
	if (!pwm.active){
		pwm.active = 1;
		// Запускаем ШИМ
		PWM5TMR = 0;
		PWM5OF  = 0;
		PWM5PH  = 0;
		PWM5LD  = 1;
		PWM5EN  = 1;
		// Запускаем COG
		G1EN    = 1;
	}
}


void pwm_set(uint16_t pr, uint16_t dc)
{
	// Вносим значения в регистры
	PWM5PR = pr;
	PWM5DC = dc;
	if (pwm.active){
		PWM5LD = 1;
		while(PWM5LD);
	}
}


void pwm_set_mode(uint8_t mode)
{
	COG1CON0bits.MD = mode;
}


void pwm_stop()
{
	if (pwm.active){
		pwm.active = 0;
		// Останавливаем ШИМ
		PWM5EN = 0;
		// Останавливаем COG
		G1EN   = 0;
	}
}
