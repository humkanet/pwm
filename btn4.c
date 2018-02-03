#include "btn4.h"
#include "pwm.h"
#include "ui.h"
#include "options.h"
#include "main.h"


void btn4_event(uint8_t event)
{
	// ��������� ��������� ������
	if (event!=BUTTON_EVENT_UP) return;
	// �������� ����� ������
	opt.pwm_mode ++;
	if (opt.pwm_mode>=PWM_nMODES) opt.pwm_mode = 0;
	pwm_set_mode(PWM_MODE[opt.pwm_mode]);
	// ��������� �����
	update_pwm_mode();
}
