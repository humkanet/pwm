#include "pcf8574.h"
#include "i2c.h"


#define bv(x)   (1<<(x))

struct {
	uint8_t output;
} pcf8574;


inline void pcf8574_init()
{
	pcf8574.output = 0x00;
}


uint8_t pcf8574_get()
{
	uint8_t data = 0x00;
	if (i2c_start((PCF8574_ADDR<<1) | I2C_READ)){
		data = i2c_read(false);
	}
	i2c_stop();
	return data;
}


void pcf8574_set(uint8_t data)
{
	if (i2c_start((PCF8574_ADDR<<1) | I2C_WRITE)){
		i2c_write(data);
		pcf8574.output = data;
	}
	i2c_stop();
}


void pcf8574_set_pin(uint8_t pin, uint8_t val)
{
	uint8_t msk = (uint8_t) bv(pin);
	if (val){
		pcf8574.output |= msk;
	}
	else{
		pcf8574.output &= (uint8_t) ~msk;
	}
	pcf8574_set(pcf8574.output);
}


inline uint8_t pcf8574_get_pin(uint8_t pin)
{
	return (uint8_t) (pcf8574_get() & bv(pin));
}
