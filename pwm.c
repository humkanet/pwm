#include <xc.h>
#include "pre.h"
#include "pwm.h"

#define GATEA_PORT  C
#define GATEA_PIN   5
#define GATEB_PORT  C
#define GATEB_PIN   4

#define U16(x)      ((uint16_t) (x))

struct {
	union {
		struct {
			uint8_t  active : 1;
		};
		uint8_t      flags;
	};
} pwm;


void pwm_init()
{
	pwm.active = 0;
	// Настраиваем пины
	LAT(GATEA_PORT, GATEA_PIN)  = 0;
	LAT(GATEB_PORT, GATEB_PIN)  = 0;
	TRIS(GATEA_PORT, GATEA_PIN) = 0;
	TRIS(GATEB_PORT, GATEB_PIN) = 0;
	SLR(GATEA_PORT, GATEA_PIN)  = 0;
	SLR(GATEB_PORT, GATEB_PIN)  = 0;
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
	PPS(GATEA_PORT, GATEA_PIN) = 0x04;
	PPS(GATEB_PORT, GATEB_PIN) = 0x05;
	// Настраиваем COG
	COG1CON0   = 0x00;
	COG1CON1   = 0xC0;
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


/* Установка полярности сигналов */
void pwm_set_polarity(uint8_t val)
{
	COG1CON1bits.POLA = val & 0x01;
	COG1CON1bits.POLB = (val>>1) & 0x01;
}


/* Установка DEADTIME */
void pwm_set_dt(uint8_t rise, uint8_t fall)
{
	COG1DBR = rise;
	COG1DBF = fall;
	G1LD    = 1;
	while (pwm.active && G1LD);
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
