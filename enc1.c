#include "enc1.h"
#include "options.h"
#include "main.h"
#include "pwm.h"


#define U32(x)  ((uint32_t) (x))


inline void enc1_time(int8_t inc)
{
	uint16_t step = T_STEP[opt.toff_step];
	// ���������� Toff
	if (inc>0){
		// ������ ������������� �������
		if (opt.toff>=T_OFF_MAX) return;
		// ����������� ����� �����
		step *= inc;
		uint16_t left = T_OFF_MAX-opt.toff;
		if (step>left) step = left;
		opt.toff += step;
	}
	// ���������� Toff
	else{
		// ������ ������������ �������
		if (opt.toff<=T_MIN) return;
		// ����������� ����� ��������
		step *= -inc;
		uint16_t left = opt.toff-T_MIN;
		if (step>left) step = left;
		opt.toff -= step;
	}
	// ������������ Ton
	if ((opt.toff+opt.ton)>T_MAX) opt.ton = T_MAX-opt.toff;
	// ������������ pr/dc
	opt.pr = us2pr(opt.ton+opt.toff);
	opt.dc = us2pr(opt.ton);
	// ��������� �����
	update_ton();
	update_toff();
}


inline void enc1_freq(int8_t inc)
{
	// ����������� ����� �������� �������
	uint32_t freq = pr2freq(opt.pr);
	uint32_t step = FREQ_STEP[opt.freq_step];
	uint16_t pr;
	// ���������� �������
	if (inc>0){
		// ��������� ����. �������
		if (freq>=FREQ_MAX) return;
		// �������� �������
		step *= inc;
		uint32_t left = FREQ_MAX-freq;
		if (step>left) step = left;
		freq += step;
		pr    = freq2pr(freq);
		// ������������� ������ ���� �� ���� ���
		if (pr==opt.pr) pr --;
	}
	// ���������� �������
	else{
		// ������ ���. �������
		if (freq<FREQ_MIN) return;
		// �������� �������
		step *= -inc;
		uint32_t left = freq-FREQ_MIN;
		if (step>left) step = left;
		freq -= step;
		pr    = freq2pr(freq);
		// ������������� ������ ���� �� ���� ���
		if (pr==opt.pr) pr ++;
	}
	// ����������� ����������
	opt.dc = (U32(opt.dc)*pr)/opt.pr;
	opt.pr = pr;
	if (!opt.dc) opt.dc ++;
	// ������������� ��������� ���
	pwm_set(opt.pr, opt.dc);
	// ��������� �����
	update_freq();
	update_duty();
}


inline void enc1_dt(int8_t inc)
{
	// ���������� ��������
	if (inc>0){
		if (opt.dtr<DT_MAX){
			uint8_t left = DT_MAX-opt.dtr;
			if (inc>left) inc = left;
		}
		else return;
	}
	// ���������� ��������
	else{
		if (opt.dtr>DT_MIN){
			uint8_t left = opt.dtr-DT_MIN;
			if (-inc>left) inc = -left;			
		}
		else return;
	}
	// ������������� ��������
	opt.dtr += inc;
	pwm_set_dt(opt.dtr, opt.dtf);
	// ��������� �����
	update_dtr();
}


void enc1_event(int8_t inc)
{
	// ����� ������: �������/����������
	if (opt.ctrl_mode==CTRL_MODE_FREQ){
		enc1_freq(inc);
	}
	// ����� ������: Ton/Toff
	else if (opt.ctrl_mode==CTRL_MODE_TIME){
		enc1_time(inc);
	}
	// ����� ������: Deadtime
	else if (opt.ctrl_mode==CTRL_MODE_DT){
		enc1_dt(inc);
	}
}
