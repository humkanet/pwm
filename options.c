#include "options.h"
#include "pwm.h"


OPTIONS opt;


const char* CTRL_MODES_LINE1[CTRL_nMODES] = {
	"   /   us",
	"   Hz/  %",
	" DEADTIME"
};
const char* CTRL_MODES_LINE2[CTRL_nMODES] = {
	"       /      us",
	"        Hz @   %",
	"        /     ns",
};


const char* POLARITY_NAMES[POLARITY_nMODES] = {
	"++",  // 0, GATEA+, GATEB+
	"-+",  // 1, GATEA-, GATEB+
	"+-",  // 2, GATEA+, GATEB-
	"--"	 // 3, GATEA-, GATEB-
};


const uint16_t FREQ_STEP[FREQ_nSTEPS] = {
	1,      // x1
	10,     // x10
	100,    // x100,
	1000,   // x1000
	10000,  // x10000
	50000,  // x50000
};
const char* FREQ_STEP_NAMES[FREQ_nSTEPS] = {
	"  1",
	" 10",
	"100",
	" 1k",
	"10k",
	"50k"            
};


const uint8_t DUTY_STEP[DUTY_nSTEPS] = {
	1,  // x1
	5,  // x5
	10  // x10
};
const char* DUTY_STEP_NAMES[DUTY_nSTEPS] = {
	" 1",
	" 5",
	"10"
};


const uint8_t T_STEP[T_nSTEPS] = {
	1,   // x1
	10,  // x10
	100  // x100
};
const char* T_STEP_NAMES[T_nSTEPS] = {
	"  1",
	" 10",
	"100"
};


const uint8_t PWM_MODE[PWM_nMODES] = {
	0b000,  // Steered PWM
	0b101,  // Push-Pull
	0b100   // Half-Bridge
};
const char* PWM_MODE_NAMES[PWM_nMODES] = {
	"PWM",
	"P-P",
	"HLF"
};


/* Время (мкс) -> значение регистра */
uint16_t us2pr(uint16_t us)
{
	uint16_t pr = TICKS_1US*us;
	return pr ? pr-1 : 0;
}


/* Значение регистра -> Время (мкс) */
uint16_t pr2us(uint16_t pr)
{
	return (pr+1)/TICKS_1US;
}


/* Частота (Гц) -> значение регистра */
uint16_t freq2pr(uint32_t freq)
{
	return FREQ2PR(freq);
}


/* Значение регистра -> частота (Гц) */
uint32_t pr2freq(uint16_t pr)
{
	return PR2FREQ(pr);
}


/* Установка параметров ШИМ */
void set_pwm(uint16_t pr, uint16_t dc)
{
	// Проверяем PR
	if (pr<PR_MIN) pr = PR_MIN;
	else if (pr>PR_MAX) pr = PR_MAX;
	// Проверяем DC
	uint16_t dc_max = pr-DC_MIN;
	if (dc<DC_MIN) dc = DC_MIN;
	else if (dc>dc_max) dc = dc_max;
	// Настраиваем ШИМ
	pwm_set(pr, dc);
	// Сохраняем параметры
	opt.pr = pr;
	opt.dc = dc;
}
