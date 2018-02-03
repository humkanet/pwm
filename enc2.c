#include "enc2.h"
#include "options.h"
#include "main.h"
#include "pwm.h"


#define U32(x)  ((uint32_t) (x))


inline void enc2_time(int8_t inc)
{
	uint16_t step = T_STEP[opt.ton_step];
	// ���������� Ton
	if (inc>0){
		// ������ ������������� �������
		if (opt.ton>=T_ON_MAX) return;
		// ����������� ����� �����
		step *= inc;
		uint16_t left = T_ON_MAX-opt.ton;
		if (step>left) step = left;
		opt.ton += step;
	}
	// ���������� Ton
	else{
		// ������ ������������ �������
		if (opt.ton<=T_MIN) return;
		// ����������� ����� ��������
		step *= -inc;
		uint16_t left = opt.ton-T_MIN;
		if (step>left) step = left;
		opt.ton -= step;
	}
	// ������������ Toff
	if ((opt.toff+opt.ton)>T_MAX) opt.toff = T_MAX-opt.ton;
	// ������������ pr/dc
	opt.pr = us2pr(opt.ton+opt.toff);
	opt.dc = us2pr(opt.ton);
	// ��������� �����
	update_ton();
	update_toff();
}


inline void enc2_freq(int8_t inc)
{
	// ����������� ����� �������� ����������
	uint8_t  duty = (U32(100)*(opt.dc+1))/opt.pr;
	uint16_t step = DUTY_STEP[opt.duty_step];
	uint16_t dc;
	if (!duty) duty ++;
	// ���������� �������
	if (inc>0){
		// ��������� ����. �������
		if (duty>=DUTY_MAX) return;
		// �������� �������
		step *= inc;
		uint16_t left = DUTY_MAX-duty;
		if (step>left) step = left;
		duty += step;
	}
	// ���������� �������
	else{
		// ������ ���. �������
		if (duty<DUTY_MIN) return;
		// �������� �������
		step *= -inc;
		uint16_t left = duty-DUTY_MIN;
		if (step>left) step = left;
		duty -= step;
	}
	// ����������� dc
	uint16_t dc = (U32(duty)*opt.pr)/100;
	if (dc==opt.dc){
		if (inc>0) dc ++;
		else dc --;
	}
	opt.dc = dc;
	if (opt.dc==opt.pr) opt.dc --;
	if (!opt.dc) opt.dc ++;
	// ������������� ��������� ���
	pwm_set(opt.pr, opt.dc);
	// ��������� �����
	update_freq();
	update_duty();
}


inline void enc2_dt(int8_t inc)
{
	// ���������� ��������
	if (inc>0){
		if (opt.dtf<DT_MAX){
			uint8_t left = DT_MAX-opt.dtf;
			if (inc>left) inc = left;
		}
		else return;
	}
	// ���������� ��������
	else{
		if (opt.dtf>DT_MIN){
			uint8_t left = opt.dtf-DT_MIN;
			if (-inc>left) inc = -left;			
		}
		else return;
	}
	// ������������� ��������
	opt.dtf += inc;
	pwm_set_dt(opt.dtr, opt.dtf);
	// ��������� �����
	update_dtf();
}


void enc2_event(int8_t inc)
{
	// ����� ������: �������/����������
	if (opt.ctrl_mode==CTRL_MODE_FREQ){
		enc2_freq(inc);
	}
	// ����� ������: Ton/Toff
	else if (opt.ctrl_mode==CTRL_MODE_TIME){
		enc2_time(inc);
	}
	// ����� ������: Deadtime
	else if (opt.ctrl_mode==CTRL_MODE_DT){
		enc2_dt(inc);
	}
}
