#include <string.h>
#include <xc.h>
#include "main.h"
#include "ui.h"
#include "pwm.h"
#include "i2c.h"
#include "pcf8574.h"
#include "lcd.h"
#include "str.h"
#include "options.h"
#include "vbat.h"


#define U16(x)     ((uint16_t) (x))
#define U32(x)     ((uint32_t) (x))


void interrupt isr()
{
    ui_isr();
}


/* Обновление полярности сигналов */
void update_polarity()
{
	lcd_goto(0, 0);
	lcd_puts((char*) POLARITY_NAMES[opt.polarity]);
}


/* Обновление режима регулировки */
void update_ctrl_mode()
{
	// Первая строка
	lcd_goto(7, 0);
	lcd_puts((char*) CTRL_MODES_LINE1[opt.ctrl_mode]);
	// Вторая строка
	lcd_goto(0, 1);
	lcd_puts((char*) CTRL_MODES_LINE2[opt.ctrl_mode]);
	// Режим работы: Ton/Toff
	if (opt.ctrl_mode==CTRL_MODE_TIME){
		update_ton_step();
		update_toff_step();
		update_ton();
		update_toff();
	}
	// Режим работы: частота/скважность
	else if (opt.ctrl_mode==CTRL_MODE_FREQ){
		update_freq_step();
		update_duty_step();
		update_freq();
		update_duty();
	}
	// Режим работы
	else if (opt.ctrl_mode==CTRL_MODE_DT){
		update_dtr();
		update_dtf();
	}
}


/*
	Обновление Deadtime
*/
void update_dtr()
{
	char str[4];
	itoa_pad(str, U16(5)*opt.dtr, 3, ITOA_TERMINATE);
	lcd_goto(4, 1);
	lcd_puts(str);
}
void update_dtf()
{
	char str[4];
	itoa_pad(str, U16(5)*opt.dtf, 3, ITOA_TERMINATE);
	lcd_goto(10, 1);
	lcd_puts(str);
}


/* Обновление режима работы ШИМ */
void update_pwm_mode()
{
	lcd_goto(3, 0);
	lcd_puts((char*) PWM_MODE_NAMES[opt.pwm_mode]);
}


/*
	Обновление шага Ton/Toff
*/
void update_toff_step()
{
	lcd_goto(7, 0);
	lcd_puts((char*) T_STEP_NAMES[opt.toff_step]);
}
void update_ton_step()
{

	lcd_goto(11, 0);
	lcd_puts((char*) T_STEP_NAMES[opt.ton_step]);
}


/*
	Обновление шага частоты/скважности
*/
void update_freq_step()
{
	lcd_goto(7, 0);
	lcd_puts((char*) FREQ_STEP_NAMES[opt.freq_step]);
}
void update_duty_step()
{
	lcd_goto(13, 0);
	lcd_puts((char*) DUTY_STEP_NAMES[opt.duty_step]);
}


/*
	Обновление Ton/Toff
*/
void update_toff()
{
	char str[5];
	itoa_pad(str, pr2us(opt.pr-opt.dc), 4, ITOA_TERMINATE);
	lcd_goto(2, 1);
	lcd_puts(str);
}
void update_ton()
{
	char str[5];
	itoa_pad(str, pr2us(opt.dc), 4, ITOA_TERMINATE);
	lcd_goto(9, 1);
	lcd_puts(str);
}


/*
	Обновление частоты/скважности
*/
void update_freq()
{
	char buf[8];
	uint32_t freq = pr2freq(opt.pr);
	if (freq>999999) freq = 999999;
	// <10kHz
	if (freq<10000){
		itoa_pad(buf, freq, 7, ITOA_TERMINATE);
	}
	// >=10kHz
	else{
		freq /= 100;
		itoa_pad(buf, freq, 5, 0);
		buf[7] = 0x00;
		buf[6] = 'k';
		buf[5] = buf[4];
		buf[4] = '.';
	}
	// Выводим данные
	lcd_goto(1, 1);
	lcd_puts(buf);
}
void update_duty()
{
	char str[3];
	uint8_t duty = (100UL*opt.dc)/opt.pr;
	itoa_pad(str, duty,  2, ITOA_TERMINATE);
	lcd_goto(13, 1);
	lcd_puts(str);
}


inline void setup()
{
	// Разрешаем pull-up
	WPUA   = 0x00;
	WPUC   = 0x00;
	nWPUEN = 0;
	// Инициализируем переферию
	ui_init();
	pwm_init();
	i2c_init();
	vbat_init();
	pcf8574_init();
	lcd_init(LCD_DISP_ON);
	lcd_led(1);
	// Настройки по-умолчанию
	opt.ctrl_mode = CTRL_MODE_FREQ;
	opt.pwm_mode  = 0;
	opt.polarity  = 0;
	opt.ton_step  = 0;
	opt.toff_step = 0;
	opt.freq_step = 3;
	opt.duty_step = 0;
	opt.dtr       = 63;
	opt.dtf       = 63;
	opt.ton       = 1;
	opt.toff      = 99;
	opt.pr        = FREQ2PR(10000);
	opt.duty      = DUTY_MIN;
	opt.dc        = (FREQ2PR(10000)*DUTY_MIN)/100;
	// Разрешаем прерывания IOC
	IOCIE = 1;
}


void vbat()
{
	static char buf[17] = " Battery: x.xxV ";
	// Измеряем напряжение аккумулятора
	uint16_t vbat = vbat_read();
	itoa_pad(buf+10, vbat, 3, 0);
	buf[13] = buf[12];
	buf[12] = buf[11];
	buf[11] = '.';
	lcd_goto(0, 0);
	lcd_puts(buf);
}


void main(void)
{
	// Запрещаем прерывания
	di();
	// Инициализация
	setup();
	// Разрешаем прерывания
	ei();

	// Отображаем напряжение аккумулятора
	lcd_clear(' ');
	vbat();
	__delay_ms(1000);
	lcd_clear(' ');

	// Настраиваем и запускаем ШИМ
	set_pwm(opt.pr, opt.dc);
	pwm_set_mode(opt.pwm_mode);
	pwm_set_dt(opt.dtr, opt.dtf);
	pwm_set_polarity(opt.polarity);
	pwm_start();

	// Обновляем экран
	update_ctrl_mode();
	update_pwm_mode();
	update_polarity();

	// Основной цикл
	for(;;){
		ui_tick();
	}
}
