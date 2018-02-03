#include "btn5.h"
#include "ui.h"
#include "options.h"
#include "main.h"


void btn5_event(uint8_t event)
{
	// ��������� ��������� ������
	if (event!=BUTTON_EVENT_UP) return;
	// �������� ����� ������
	opt.ctrl_mode ++;
	if (opt.ctrl_mode>=CTRL_nMODES){
		opt.ctrl_mode = 0;
	}
	// ��������� �����
	update_ctrl_mode();
}
