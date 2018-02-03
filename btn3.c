#include <xc.h>
#include "ui.h"
#include "btn3.h"
#include "options.h"
#include "main.h"


void btn3_event(uint8_t event)
{
	// Проверяем состояние кнопки
	if (event!=BUTTON_EVENT_UP) return;
	// Режим работы: частота/скважность
	if (opt.ctrl_mode==CTRL_MODE_FREQ){
		opt.freq_step ++;
		if (opt.freq_step>=FREQ_nSTEPS) opt.freq_step = 0;
		update_freq_step();
	}
	// Режим работы: Ton/Toff
	else if (opt.ctrl_mode==CTRL_MODE_TIME){
		opt.toff_step ++;
		if (opt.toff_step>=T_nSTEPS) opt.toff_step = 0;
		update_toff_step();
	}
}
