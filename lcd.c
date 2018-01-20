#include <xc.h>
#include "lcd.h"
#include "pcf8574.h"

#if LCD_LINES==1
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_4BIT_1LINE 
#else
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_4BIT_2LINES 
#endif

#define bv(x)          (1<<(x))
#define LCD_DATA_MASK  (bv(LCD_DATA7_PIN) | bv(LCD_DATA6_PIN) | bv(LCD_DATA5_PIN) | bv(LCD_DATA4_PIN))

#define lcd_e_delay()  asm("nop"); asm("nop"); asm("nop"); asm("nop")


struct {
	uint8_t port;
} lcd;


void lcd_e_toggle()
{
	pcf8574_set_pin(LCD_E_PIN, 1);
	lcd_e_delay();
	pcf8574_set_pin(LCD_E_PIN, 0);
}


void lcd_init(uint8_t params)
{
	lcd.port = 0x00;
	pcf8574_set(lcd.port);
	// Сбрасываем экран
	__delay_ms(16);
	lcd.port = bv(LCD_DATA5_PIN) | bv(LCD_DATA4_PIN);
	pcf8574_set(lcd.port);
	lcd_e_toggle();
	__delay_ms(5);
	lcd_e_toggle();      
	__delay_ms(5);
	lcd_e_toggle();      
	__delay_ms(5);
	// Переводим в 4-битный режим
	lcd.port &= ~bv(LCD_DATA4_PIN);
	pcf8574_set(lcd.port);
	lcd_e_toggle();
	__delay_ms(64);
	// Инициализируем
	lcd_command(LCD_FUNCTION_DEFAULT);
	lcd_command(LCD_DISP_OFF);
	lcd_command(LCD_MODE_DEFAULT);
	lcd_command(params);
}


uint8_t lcd_wait()
{
	while(lcd_read(0) & bv(LCD_BUSY));
	__delay_us(4);
	return lcd_read(0);
}


void lcd_write(uint8_t data, uint8_t rs) 
{
	if (rs) lcd.port |= (1<<LCD_RS_PIN);
	else lcd.port &= ~(1<<LCD_RS_PIN);
	lcd.port &= ~bv(LCD_RW_PIN);
	pcf8574_set(lcd.port);
	// Передаем 8 бит
	for (uint8_t n=0; n<2; n++){
		lcd.port &= ~LCD_DATA_MASK;
		if(data & 0x80) lcd.port |= bv(LCD_DATA7_PIN);
		if(data & 0x40) lcd.port |= bv(LCD_DATA6_PIN);
		if(data & 0x20) lcd.port |= bv(LCD_DATA5_PIN);
		if(data & 0x10) lcd.port |= bv(LCD_DATA4_PIN);
		pcf8574_set(lcd.port);
		lcd_e_toggle();
		data <<= 4;
	}
	//
	lcd.port |= LCD_DATA_MASK;
	pcf8574_set(lcd.port);
}


uint8_t lcd_read(uint8_t rs) 
{
	uint8_t tmp;
	uint8_t data;
	if (rs) lcd.port |= bv(LCD_RS_PIN);
	else lcd.port &= ~bv(LCD_RS_PIN);
	lcd.port |= bv(LCD_RW_PIN);
	pcf8574_set(lcd.port);
	// Считываем 8 бит
	data = 0x00;
	for(uint8_t n=0; n<2; n++){
		data <<= 4;
		pcf8574_set_pin(LCD_E_PIN, 1);
		lcd_e_delay();
		tmp = pcf8574_get();
		if (tmp & bv(LCD_DATA7_PIN)) data |= 0x08;
		if (tmp & bv(LCD_DATA6_PIN)) data |= 0x04;
		if (tmp & bv(LCD_DATA5_PIN)) data |= 0x02;
		if (tmp & bv(LCD_DATA4_PIN)) data |= 0x01;
		pcf8574_set_pin(LCD_E_PIN, 0);
		lcd_e_delay();
	}
	// Возвращаем результат
	return data;
}


void lcd_command(uint8_t cmd)
{
	lcd_wait();
	lcd_write(cmd,0);
}


void lcd_goto(uint8_t x, uint8_t y)
{
	uint8_t cmd = (uint8_t) (bv(LCD_DDRAM) + x);
#if LCD_LINES==1
	cmd += LCD_START_LINE1;
#elif LCD_LINES==2
	cmd += (uint8_t) (y ? LCD_START_LINE2 : LCD_START_LINE1);
#elif LCD_LINES==4
	if      (y==0) cmd += LCD_START_LINE1;
	else if (y==1) cmd += LCD_START_LINE2;
	else if (y==2) cmd += LCD_START_LINE3;
	else cmd += LCD_START_LINE4;
#endif
	lcd_command(cmd);
}


void lcd_puts(char *str)
{
	while(*str){
		lcd_wait();
		lcd_write(*str++, 1);
	}
}


void lcd_led(uint8_t on)
{
	if (on) lcd.port |= bv(LCD_LED_PIN);
	else lcd.port &= ~bv(LCD_LED_PIN);
	pcf8574_set(lcd.port);
}
