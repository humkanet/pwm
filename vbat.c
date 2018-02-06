#include <xc.h>
#include "vbat.h"

#define U32(x)  ((uint32_t) (x))


void vbat_init()
{
	// Настраиваем ИОН (2.048V)
	FVRCON = 0b00000010;
	// Настраиваем ЦАП (Vcc/4)
	DAC3CON0 = 0b00000000;
	DAC3REF  = 32/4;
}


uint16_t vbat_read()
{
	// Сохраняем настройки АЦП
	uint8_t con0 = ADCON0;
	uint8_t con1 = ADCON1;
	// Включаем ИОН и ЦАП
	FVREN  = 1;
	DAC3EN = 1;
	// Перенастраиваем АЦП
	ADCON0 = 0b01101100;
	ADCON1 = 0b01110011;
	ADON   = 1;
	__delay_us(300);
	// Измеряем 4 раза напряжение
	uint16_t val = 0;
	for(uint8_t n=0; n<4; n++){
		GO_nDONE = 1;
		while(GO_nDONE);
		val += ADRESH;
	}
	/*
		Переводим в значение с 2 знаками после запятой:
		V/4 = Vref*(adc/4)/256 = 2048*adc/1024 = adc*2
		V   = adc*8
		V   = adc<<3;
	*/
	val = (val<<3)/10;
	// Восстанавливаем настройки АЦП
	ADCON0 = con0;
	ADCON1 = con1;
	// Отключаем ИОН и ЦАП
	FVREN  = 0;
	DAC3EN = 0;
	// Возвращаем значение
	return val;
}
