#include <xc.h>
#include "i2c.h"
#include "pre.h"


#define SDA_PORT  A
#define SDA_PIN   0
#define SCL_PORT  A
#define SCL_PIN   1


#define i2c_wait(mask)  while((SSPCON2 & 0x1F) | (SSPSTAT & mask))


void i2c_init()
{
	// Отключаем MSSP
	SSPEN      = 0;
	// Настраиваем пины
	ANS(SDA_PORT, SDA_PIN)  = 0;
	ANS(SCL_PORT, SCL_PIN)  = 0;
	TRIS(SDA_PORT, SDA_PIN) = 1;
	TRIS(SCL_PORT, SCL_PIN) = 1;
	WPU(SDA_PORT, SDA_PIN)  = 1;
	WPU(SCL_PORT, SCL_PIN)  = 1;
	PPS(SDA_PORT, SDA_PIN)  = 0x13;
	PPS(SCL_PORT, SCL_PIN)  = 0x12;
	SSPCLKPPS = 0x01;
	SSPDATPPS = 0x00;
	// Настраиваем MSSP
	SSPADD     = _XTAL_FREQ/(4*I2C_SPEED)-1;
	SSPSTAT    = 0x80;
	SSPCON2    = 0x00;
	SSPCON3    = 0x00;
	SSPCON1    = 0x28;
}


void i2c_off()
{
	// Отключаем MSSP
	SSPCON1  = 0x00;
	// Настраиваем пины
	TRIS(SDA_PORT, SDA_PIN) = 1;
	TRIS(SDA_PORT, SDA_PIN) = 1;
	ANS(SDA_PORT, SDA_PIN)  = 1;
	ANS(SCL_PORT, SCL_PIN)  = 1;
	WPU(SDA_PORT, SDA_PIN)  = 0;
	WPU(SCL_PORT, SCL_PIN)  = 0;	
}


bool i2c_start(uint8_t addr)
{
	// Start i2c
	SSPCON2bits.SEN = 1;
	while(SSPCON2bits.SEN);
	// Send device address    
	i2c_write(addr);
	return SSPCON2bits.ACKSTAT==0;
}


bool i2c_restart(uint8_t addr)
{
	// ReStart i2c
	SSPCON2bits.RSEN = 1;
	while (SSPCON2bits.RSEN);
	// Send device address
	i2c_write(addr);
	return SSPCON2bits.ACKSTAT==0;
}


void i2c_stop()
{
	SSPCON2bits.PEN = 1;
	while(SSPCON2bits.PEN);
}


void i2c_write(uint8_t val)
{
	SSPBUF = val;
	while (SSPSTATbits.BF);
	i2c_wait(0x04);
}


void i2c_write_data(uint8_t *data, uint8_t len)
{
	while(len--){
		i2c_write(*data++);
	}
}


uint8_t i2c_read(bool ack)
{
	uint8_t val;
	SSPCON2bits.RCEN = 1;
	while (!SSPSTATbits.BF);
	val    = SSPBUF;
	i2c_wait(0x04);
	// ack/nack
	ACKDT  = ack ? 0 : 1;
	ACKEN  = 1;
	while (SSPCON2bits.ACKEN);
	return val;
}


void i2c_read_data(uint8_t *data, uint8_t len)
{
	while (--len){
		*data++ = i2c_read(true);
	}
	*data = i2c_read(false);
}
