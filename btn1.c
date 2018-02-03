#include <xc.h>
#include "ui.h"
#include "btn1.h"
#include "options.h"
#include "pwm.h"
#include "main.h"


void btn1_event(uint8_t event)
{
	// ��������� ��������� ������
	if (event!=BUTTON_EVENT_UP) return;
	// ����������� ���������� ��������
	opt.polarity ++;
	if (opt.polarity>=POLARITY_nMODES) opt.polarity = 0;
	// ����������� ���
	pwm_set_polarity(opt.polarity);
	// ��������� ���������� �� ������
	update_polarity();
}
