#include <xc.h>
#include "ui.h"
#include "btn1.h"
#include "options.h"
#include "pwm.h"
#include "main.h"


void btn1_event(uint8_t event)
{
	// Проверяем состояние кнопки
	if (event!=BUTTON_EVENT_UP) return;
	// Переключаем полярность сигналов
	opt.polarity ++;
	if (opt.polarity>=POLARITY_nMODES) opt.polarity = 0;
	// Настраиваем ШИМ
	pwm_set_polarity(opt.polarity);
	// Обновляем информацию на экране
	update_polarity();
}
