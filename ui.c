#include <xc.h>
#include "ui.h"
#include "btn1.h"
#include "btn2.h"
#include "btn3.h"
#include "btn4.h"
#include "btn5.h"
#include "enc1.h"
#include "enc2.h"
#include "pre.h"


#define UI_nENCODERS  2
#define UI_nBUTTONS   5

// Энкодер1
#define ENC1_PORTA    C
#define ENC1_PINA     1
#define ENC1_PORTB    C
#define ENC1_PINB     0

// Энкодер2
#define ENC2_PORTA    C
#define ENC2_PINA     3
#define ENC2_PORTB    C
#define ENC2_PINB     2

// Настройка кнопок
#define ADC_BUTTONS_PORT   A
#define ADC_BUTTONS_PIN    2
#define ADC_BUTTONS_CHS    0b00010


#define BUTTON_STAGE_IDLE  0x00
#define BUTTON_STAGE_DOWN  0x01
#define BUTTON_NOTHING     0xFF
#define BUTTON_ERROR       0xFE


typedef struct {
	uint8_t       stage;
	BUTTON_EVENT  event;
} BUTTON;


typedef struct {
	int8_t         inc;
	ENCODER_EVENT  event;
} ENCODER;


struct {
	BUTTON            buttons[UI_nBUTTONS];
	volatile ENCODER  encoders[UI_nENCODERS];
} ui = 0;


const uint8_t ADC_BUTTONS[UI_nBUTTONS][2] = {
	{   0,   0},
	{  44,  55},
	{  93, 112},
	{ 152, 174},
	{ 201, 221}
};


inline void ui_isr()
{
	// Обрабатываем энкодер1
	if (IOCF(ENC1_PORTA, ENC1_PINA)){
		// Сбрасываем флаг прерывания
		IOCF(ENC1_PORTA, ENC1_PINA) = 0;
		// Запоминаем значение
		int8_t inc = ui.encoders[0].inc;
		inc += PORT(ENC1_PORTB, ENC1_PINB) ? -1 : 1;
		if (inc>100) inc = 100;
		else if (inc<-100) inc = -100;
		ui.encoders[0].inc = inc;
	}
	// Обрабатываем энкодер2
	if (IOCF(ENC2_PORTA, ENC2_PINA)){
		// Сбрасываем флаг прерывания
		IOCF(ENC2_PORTA, ENC2_PINA) = 0;
		// Запоминаем значение
		int8_t inc = ui.encoders[1].inc;
		inc += PORT(ENC2_PORTB, ENC2_PINB) ? -1 : 1;
		if (inc>100) inc = 100;
		else if (inc<-100) inc = -100;
		ui.encoders[1].inc = inc;
	}
}


inline void ui_init()
{
	// Настраиваем энкодеры
	for (uint8_t n=0; n<UI_nENCODERS; n++){
		ui.encoders[n].inc   = 0;
		ui.encoders[n].event = 0;
	}
	// Настраиваем пины
	ANS(ENC1_PORTA, ENC1_PINA)  = 0;
	ANS(ENC1_PORTB, ENC1_PINB)  = 0;
	TRIS(ENC1_PORTA, ENC1_PINA) = 1;
	TRIS(ENC1_PORTB, ENC1_PINB) = 1;
	WPU(ENC1_PORTA, ENC1_PINA)  = 1;
	WPU(ENC1_PORTB, ENC1_PINB)  = 1;
	IOCP(ENC1_PORTA, ENC1_PINA) = 1;
	IOCN(ENC1_PORTA, ENC1_PINA) = 0;
	IOCF(ENC1_PORTA, ENC1_PINA) = 0;
	ANS(ENC2_PORTA, ENC2_PINA)  = 0;
	ANS(ENC2_PORTB, ENC2_PINB)  = 0;
	TRIS(ENC2_PORTA, ENC2_PINA) = 1;
	TRIS(ENC2_PORTB, ENC2_PINB) = 1;
	WPU(ENC2_PORTA, ENC2_PINA)  = 1;
	WPU(ENC2_PORTB, ENC2_PINB)  = 1;
	IOCP(ENC2_PORTA, ENC2_PINA) = 1;
	IOCN(ENC2_PORTA, ENC2_PINA) = 0;
	IOCF(ENC2_PORTA, ENC2_PINA) = 0;
	// Настраиваем АЦП
	ADCON0 = 0x00;
	ADCON1 = 0b01110000;
	ADON   = 1;
	// Настраиваем пин кнопок
	TRIS(ADC_BUTTONS_PORT, ADC_BUTTONS_PIN) = 1;
	ANS(ADC_BUTTONS_PORT, ADC_BUTTONS_PIN)  = 1;
	WPU(ADC_BUTTONS_PORT, ADC_BUTTONS_PIN)  = 0;
	// Привязываем кнопки и энкодеры
	ui_button_event(0, btn1_event);
	ui_button_event(1, btn2_event);
	ui_button_event(2, btn3_event);
	ui_button_event(3, btn4_event);
	ui_button_event(4, btn5_event);
	ui_encoder_event(0, enc1_event);
	ui_encoder_event(1, enc2_event);
}


void ui_button_event(uint8_t n, BUTTON_EVENT event)
{
	if (n<UI_nBUTTONS){
		ui.buttons[n].event = event;
	}
}


void ui_encoder_event(uint8_t n, ENCODER_EVENT event)
{
	if (n<UI_nENCODERS){
		ui.encoders[n].event = event;
	}
}


inline uint8_t ui_button()
{
	// Выбираем канал кнопок
	ADCON0bits.CHS = ADC_BUTTONS_CHS;
	__delay_us(50);
	// Делаем первый замер
	GO_nDONE = 1;
	while(GO_nDONE);
	uint8_t adc = ADRESH;
	// Ждем 3 подтверждения
	for (uint8_t n=0; n<3; n++){
		GO_nDONE = 1;
		while(GO_nDONE);
		uint8_t d = (ADRESH>adc) ? ADRESH-adc : adc-ADRESH;
		if (d>5) return BUTTON_ERROR;
	}
	// Определяем какая кнопка нажата
	for (uint8_t n=0; n<UI_nBUTTONS; n++){
		if ((adc>=ADC_BUTTONS[n][0]) && (adc<=ADC_BUTTONS[n][1])) return n;
	}
	// Кнопка не нажата
	return BUTTON_NOTHING;
}


void ui_tick()
{
	/* Проверяем кнопки */
	uint8_t btn = ui_button();
	if (btn==BUTTON_ERROR) return;
	// Нажата одна из кнопок
	if (btn<UI_nBUTTONS){
		BUTTON *b = &ui.buttons[btn];
		if (b->stage==BUTTON_STAGE_IDLE){
			b->stage = BUTTON_STAGE_DOWN;
			if (b->event) b->event(BUTTON_EVENT_DOWN);
		}
	}
	// Ни одна из кнопок не нажата, обрабатываем событие UP
	else{
		BUTTON *b = ui.buttons;
		for (uint8_t n=0; n<UI_nBUTTONS; n++){
			if (b->stage==BUTTON_STAGE_DOWN){
				b->stage = BUTTON_STAGE_IDLE;
				if (b->event) b->event(BUTTON_EVENT_UP);
			}
			b ++;
		}
	}

	/* Проверяем энкодеры */
	for (uint8_t n=0; n<UI_nENCODERS; n++){
		int8_t inc = ui.encoders[n].inc;	
		if(inc){
			ui.encoders[n].inc = 0;
			if(ui.encoders[n].event) ui.encoders[n].event(inc);
		}
	}
}
