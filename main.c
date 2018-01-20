#include <string.h>
#include <xc.h>
#include "main.h"
#include "ui.h"
#include "pwm.h"
#include "i2c.h"
#include "pcf8574.h"
#include "lcd.h"
#include "str.h"


#define U8(x)      ((uint8_t) (x))
#define U16(x)     ((uint16_t) (x))
#define U32(x)     ((uint32_t) (x))


struct {
	uint8_t  ton_step;
	uint8_t  toff_step;
	uint8_t  freq_step;
	uint8_t  duty_step;
	uint8_t  mode;
	uint8_t  ctrl_mode;
	uint16_t ton;        // us
	uint16_t toff;       // us
	uint32_t freq;       // Hz
	uint8_t  duty;       // %,
} run = {
	0,
	0,
	0,
	0,
	0,
	0,
	1,
	99,
	0,
	0
};


void interrupt isr()
{
    ui_isr();
}


void line1()
{
	static char mode0[] = ">xxx   xxx/xxxus";
	static char mode1[] = ">xxx xxxHz / xx%";
	char *str;
	// Режим работы
	switch(run.ctrl_mode){
		// ВРЕМЯ
		case CTRL_MODE_TIME:
			str = mode0;
			memcpy(str+7, T_STEP_NAME[run.ton_step], 3);
			memcpy(str+11, T_STEP_NAME[run.toff_step], 3);
			break;
		// ЧАСТОТА
		case CTRL_MODE_FREQ:
			str = mode1;
			memcpy(str+5, FREQ_STEP_NAME[run.freq_step], 3);
			memcpy(str+13, DUTY_STEP_NAME[run.duty_step], 2);
			break;
	}
	// Тип ШИМ
	memcpy(str+1, PWM_MODE_NAME[run.mode], 3);
	lcd_goto(0, 0);
	lcd_puts(str);
}


void line2()
{
	static char mode0[] = "123456:123456 us";
	static char mode1[] = "0123456Hz @ 50% ";
	char *str;
	if (run.ctrl_mode==CTRL_MODE_TIME){
		str = mode0;
		itoa_pad(mode0, run.ton, 6, 0);
		itoa_pad(mode0+7, run.toff, 6, 0);
	}
	else if (run.ctrl_mode==CTRL_MODE_FREQ){
		str = mode1;
		itoa_pad(mode1, run.freq, 7, 0);
		itoa_pad(mode1+12, run.duty, 2, 0);
	}
	lcd_goto(0, 1);
	lcd_puts(str);
}


void pwm_set_time(uint16_t ton, uint16_t toff)
{
	static uint16_t prev_ton  = 0;
	static uint16_t prev_toff = 0;
	if ((ton!=prev_ton) || (toff!=prev_toff)){
		prev_ton  = ton;
		prev_toff = toff;	
		ton      *= TICKS_1US;
		toff     *= TICKS_1US;
		pwm_set(ton+toff-1, ton);
		// Обновляем вторую строку
		line2();
	}
}


void pwm_set_freq(uint32_t freq, uint8_t duty)
{
	static uint32_t prev_freq = 0;
	static uint8_t  prev_duty = 0;
	if ((freq!=prev_freq) || (duty!=prev_duty)){
		uint16_t pr = (_XTAL_FREQ/freq)-1;
		uint16_t dc = U32(pr)*duty/100;
		prev_freq	  = freq;
		prev_duty   = duty;
		if (!dc){
			dc = 1;
		}
		else if (dc>=pr){
			dc = pr-1;
		}
		pwm_set(pr, dc);
		// Обновляем вторую строку
		line2();
	}
}


void btn1_event(uint8_t event)
{
	if (event==BUTTON_EVENT_UP){
		switch (run.ctrl_mode){
			// Режим работы: ВРЕМЯ
			case CTRL_MODE_TIME:
				run.toff_step ++;
				if (run.toff_step>=T_nSTEPS){
					run.toff_step = 0;
				}
				break;
			// Режим работы: ЧАСТОТА
			case CTRL_MODE_FREQ:
				run.duty_step ++;
				if (run.duty_step>=DUTY_nSTEPS){
					run.duty_step = 0;
				}
				break;
		}
		// Обновляем первую строку
		line1();
	}
}


void btn2_event(uint8_t event)
{
	if (event==BUTTON_EVENT_UP){
		switch(run.ctrl_mode){
			// Режим работы: ВРЕМЯ
			case CTRL_MODE_TIME:
				run.ton_step ++;
				if (run.ton_step>=T_nSTEPS){
					run.ton_step = 0;
				}
				break;
			// Режим работы: ЧАСТОТА
			case CTRL_MODE_FREQ:
				run.freq_step ++;
				if (run.freq_step>=FREQ_nSTEPS) run.freq_step = 0;
				break;
		}
		// Обновляем первую строку
		line1();
	}
}


void btn3_event(uint8_t event)
{
	if (event==BUTTON_EVENT_UP){
		run.mode ++;
		if (run.mode>=PWM_nMODES){
			run.mode = 0;
		}
		pwm_set_mode(PWM_MODE[run.mode]);
		line1();	
	}
}


void btn4_event(uint8_t event)
{
	if (event==BUTTON_EVENT_UP){
		run.ctrl_mode ++;
		if (run.ctrl_mode>=CTRL_nMODES){
			run.ctrl_mode = 0;
		}
		// Обновляем первую строку
		line1();
		// Пересчитываем параметры
		if (run.ctrl_mode==CTRL_MODE_FREQ){
			run.freq = 1000000UL/(run.ton + run.toff);
			run.duty = U16(100)*run.ton/(run.ton+run.toff);
			pwm_set_freq(run.freq, run.duty);
		}
		else if (run.ctrl_mode==CTRL_MODE_TIME){
			uint32_t us   = 1000000UL/run.freq;
			uint16_t ton  = us*run.duty/100;
			uint16_t toff = us - ton;
			if (ton<T_MIN){
				ton = T_MIN;
			}
			if (toff<T_MIN){
				toff = T_MIN;
			}
			run.ton  = ton;
			run.toff = toff;
			pwm_set_time(run.ton, run.toff);
		}
		// Обновляем вторую строку
		line2();
	}
}


void enc1_event(int8_t inc)
{
	/* Режим работы: ВРЕМЯ */
	if (run.ctrl_mode==CTRL_MODE_TIME){
		// Увеличение Ton
		if (inc>0){
			uint16_t left = T_ON_MAX-run.ton;
			uint16_t d    = U16(T_STEP[run.ton_step])*U8(inc);
			if (d>left) d = left;
			run.ton += d;
			// Проверяем переполнение
			if ((run.ton+run.toff)>T_MAX) run.toff = T_MAX - run.ton;
		}
		// Уменьшение Ton
		else{
			uint16_t left = run.ton - T_MIN;
			uint16_t d    = U16(T_STEP[run.ton_step])*U8(-inc);
			if (d>left) d = left;
			run.ton -= d;
		}
		pwm_set_time(run.ton, run.toff);
	}

	/* Режим работы: ЧАСТОТА */
	else if (run.ctrl_mode==CTRL_MODE_FREQ){
		uint32_t d = FREQ_STEP[run.freq_step];
		// Увеличение частоты
		if (inc>0){
			if (run.freq<FREQ_MIN) run.freq = FREQ_MIN;
			else if (run.freq<FREQ_MAX){
				uint32_t left = FREQ_MAX-run.freq;
				d *= U8(inc);
				if (d>left) d = left;		    
				run.freq += d;
			}
			else return;
		}
		// Уменьшение частоты
		else{
			if (run.freq>FREQ_MAX) run.freq = FREQ_MAX;
			else if (run.freq>FREQ_MIN){
				uint32_t left = run.freq-FREQ_MIN;
				d *= U8(-inc);
				if (d>left) d = left;
				run.freq -= d;
			}
	    else return;
		}
		// Устанавливаем частоту
		pwm_set_freq(run.freq, run.duty);
	}
}


void enc2_event(int8_t inc)
{
	/* Режим работы: ВРЕМЯ */
	if (run.ctrl_mode==CTRL_MODE_TIME){
		uint16_t d = T_STEP[run.toff_step];
		// Увеличивание Toff
		if (inc>0){
			uint16_t left = T_OFF_MAX-run.toff;
			d *= U8(inc);
			if (d>left) d = left;
			run.toff += d;
	    // Проверяем переполнение
	    if ((run.ton+run.toff)>T_MAX) run.ton = T_MAX - run.toff;
		}
		// Уменьшение Toff
		else{
			uint16_t left = run.toff - T_MIN;
			d = U8(-inc);
			if (d>left) d = left;
			run.toff -= d;
		}
		pwm_set_time(run.ton, run.toff);
	}

	/* Режим работы: ЧАСТОТА */
	else if (run.ctrl_mode==CTRL_MODE_FREQ){
		uint16_t d = DUTY_STEP[run.duty_step];
		if (inc>0){
			uint16_t left = DUTY_MAX-run.duty;
			d *= U8(inc);
			if (d>left) d = left;
			run.duty += d;
		}
		else{
			uint16_t left = run.duty-DUTY_MIN;
			d *= U8(-inc);
			if (d>left) d = left;
			run.duty -= d;
		}
		pwm_set_freq(run.freq, run.duty);
	}
}


inline void setup()
{
	// Разрешаем pull-up
	WPUA = 0x00;
	WPUC = 0x00;
	nWPUEN = 0;
	// Инициализируем переферию
	ui_init();
	pwm_init();
	i2c_init();
	pcf8574_init();
	lcd_init(LCD_DISP_ON);
	lcd_led(1);
	// Настраиваем события от кнопок/энкодеров
	ui_button_event(0, btn1_event);
	ui_button_event(1, btn2_event);
	ui_button_event(2, btn3_event);
	ui_button_event(3, btn4_event);
	ui_encoder_event(0, enc2_event);
	ui_encoder_event(1, enc1_event);
	// Разрешаем прерывания IOC
	IOCIE = 1;
}


void main(void) {
	// Запрещаем прерывания
	di();
	// Инициализация
	setup();
	// Разрешаем прерывания
	ei();

	// Запускаем ШИМ
	pwm_set_mode(run.mode);
	pwm_set_time(run.ton, run.toff);
	pwm_start();

	// Отображаем данные
	line1();
	line2();

	// Основной цикл
	for(;;){
		ui_tick();
	}
}
