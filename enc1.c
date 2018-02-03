#include "enc1.h"
#include "options.h"
#include "main.h"
#include "pwm.h"


#define U32(x)  ((uint32_t) (x))


inline void enc1_time(int8_t inc)
{
	uint16_t step = T_STEP[opt.toff_step];
	// Увеличение Toff
	if (inc>0){
		// Больше максимального времени
		if (opt.toff>=T_OFF_MAX) return;
		// Расчитываем новое время
		step *= inc;
		uint16_t left = T_OFF_MAX-opt.toff;
		if (step>left) step = left;
		opt.toff += step;
	}
	// Уменьшение Toff
	else{
		// Меньше минимального времени
		if (opt.toff<=T_MIN) return;
		// Расчитываем новое значение
		step *= -inc;
		uint16_t left = opt.toff-T_MIN;
		if (step>left) step = left;
		opt.toff -= step;
	}
	// Корректируем Ton
	if ((opt.toff+opt.ton)>T_MAX) opt.ton = T_MAX-opt.toff;
	// Рассчитываем pr/dc
	opt.pr = us2pr(opt.ton+opt.toff);
	opt.dc = us2pr(opt.ton);
	pwm_set(opt.pr, opt.dc);
	// Обновляем экран
	update_ton();
	update_toff();
}


inline void enc1_freq(int8_t inc)
{
	// Расчитываем новое значение частоты
	uint32_t freq = pr2freq(opt.pr);
	uint32_t step = FREQ_STEP[opt.freq_step];
	uint16_t pr;
	// Увеличение частоты
	if (inc>0){
		// Превышена макс. частота
		if (freq>=FREQ_MAX) return;
		// Изменяем частоту
		step *= inc;
		uint32_t left = FREQ_MAX-freq;
		if (step>left) step = left;
		freq += step;
		pr    = freq2pr(freq);
		// Принудительно делаем хотя бы один шаг
		if (pr==opt.pr) pr --;
	}
	// Уменьшение частоты
	else{
		// Меньше мин. частоты
		if (freq<FREQ_MIN) return;
		// Изменяем частоту
		step *= -inc;
		uint32_t left = freq-FREQ_MIN;
		if (step>left) step = left;
		freq -= step;
		pr    = freq2pr(freq);
		// Принудительно делаем хотя бы один шаг
		if (pr==opt.pr) pr ++;
	}
	// Расчитываем скважность
	opt.dc = (U32(opt.dc)*pr)/opt.pr;
	opt.pr = pr;
	if (!opt.dc) opt.dc ++;
	// Устанавливаем параметры ШИМ
	pwm_set(opt.pr, opt.dc);
	// Обновляем экран
	update_freq();
	update_duty();
}


inline void enc1_dt(int8_t inc)
{
	// Увеличение значения
	if (inc>0){
		if (opt.dtr<DT_MAX){
			uint8_t left = DT_MAX-opt.dtr;
			if (inc>left) inc = left;
		}
		else return;
	}
	// Уменьшение значения
	else{
		if (opt.dtr>DT_MIN){
			uint8_t left = opt.dtr-DT_MIN;
			if (-inc>left) inc = -left;			
		}
		else return;
	}
	// Устанавливаем значение
	opt.dtr += inc;
	pwm_set_dt(opt.dtr, opt.dtf);
	// Обновляем экран
	update_dtr();
}


void enc1_event(int8_t inc)
{
	// Режим работы: частота/скважность
	if (opt.ctrl_mode==CTRL_MODE_FREQ){
		enc1_freq(inc);
	}
	// Режим работы: Ton/Toff
	else if (opt.ctrl_mode==CTRL_MODE_TIME){
		enc1_time(inc);
	}
	// Режим работы: Deadtime
	else if (opt.ctrl_mode==CTRL_MODE_DT){
		enc1_dt(inc);
	}
}
