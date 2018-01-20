#include <xc.h>
#include "ui.h"
#include "pre.h"

#define UI_nENCODERS  2
#define UI_nBUTTONS   4

// Энкодер1
#define ENC1_PORTA    C
#define ENC1_PINA     3
#define ENC1_PORTB    C
#define ENC1_PINB     2

// Энкодер2
#define ENC2_PORTA    C
#define ENC2_PINA     1
#define ENC2_PORTB    C
#define ENC2_PINB     0

// Настройка кнопок
#define UI_BUTTONS_PORT    A
#define UI_BUTTONS_PIN     2
#define UI_BUTTONS_CHS     0b00010


#define BUTTON_STAGE_IDLE  0x00
#define BUTTON_STAGE_DOWN  0x01
#define BUTTON_NOTHING     0xFF

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
	// Настраиваем пин кнопок
	TRIS(UI_BUTTONS_PORT, UI_BUTTONS_PIN) = 1;
	ANS(UI_BUTTONS_PORT, UI_BUTTONS_PIN)  = 1;
	WPU(UI_BUTTONS_PORT, UI_BUTTONS_PIN)  = 0;
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
	const uint8_t buttons_adc[UI_nBUTTONS] = {
		63,
		148,
		180,
		224
	};
	// Выбираем канал кнопок
	ADCON0bits.CHS = UI_BUTTONS_CHS;
	// Запускаем измерение
	ADON    = 1;
	GO_nDONE = 1;
	while(GO_nDONE);
	// Считываем значение и отключаем АЦП
	uint8_t adc = ADRESH;
	ADON = 0;
	// Определяем какая кнопка нажата
	for (uint8_t n=0; n<UI_nBUTTONS; n++){
		if (adc<buttons_adc[n]) return n;
	}
	// Кнопка не нажата
	return BUTTON_NOTHING;
}


void ui_tick()
{
	/* Проверяем кнопки */
	uint8_t btn = ui_button();
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
