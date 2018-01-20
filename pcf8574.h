#ifndef _PCF8574_H
#define _PCF8574_H

#include <stdint.h>

#define PCF8574_ADDR     (0x3F)

inline void     pcf8574_init();
uint8_t         pcf8574_get();
void            pcf8574_set(uint8_t data);
void            pcf8574_set_pin(uint8_t pin, uint8_t val);
inline uint8_t  pcf8574_get_pin(uint8_t pin);

#endif
