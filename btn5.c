#include "btn5.h"
#include "ui.h"
#include "options.h"
#include "main.h"


void btn5_event(uint8_t event)
{
	// Проверяем состояние кнопки
	if (event!=BUTTON_EVENT_UP) return;
	// Изменяем режим работы
	opt.ctrl_mode ++;
	if (opt.ctrl_mode>=CTRL_nMODES){
		opt.ctrl_mode = 0;
	}
	// Обновляем экран
	update_ctrl_mode();
}
