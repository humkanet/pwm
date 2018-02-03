#include <xc.h>
#include "ui.h"
#include "btn2.h"
#include "options.h"
#include "main.h"


void btn2_event(uint8_t event)
{
	// Проверяем состояние кнопки
	if (event!=BUTTON_EVENT_UP) return;
	// Режим работы: частота/скважность
	if (opt.ctrl_mode==CTRL_MODE_FREQ){
		opt.duty_step ++;
		if (opt.duty_step>=DUTY_nSTEPS) opt.duty_step = 0;
		update_duty_step();
	}
	// Режим работы: Ton/Toff
	else if (opt.ctrl_mode==CTRL_MODE_TIME){
		opt.ton_step ++;
		if (opt.ton_step>=T_nSTEPS) opt.ton_step = 0;
		update_ton_step();
	}
}
