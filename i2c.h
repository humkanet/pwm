#ifndef _I2C_H
#define	_I2C_H

#include <stdint.h>
#include <stdbool.h>

#define I2C_SPEED  100000U
#define I2C_WRITE  0x00U
#define I2C_READ   0x01U

void     i2c_init();
void     i2c_off();
bool     i2c_start(uint8_t addr);
bool     i2c_restart(uint8_t addr);
void     i2c_stop();
void     i2c_write(uint8_t val);
uint8_t  i2c_read(bool ack);
void     i2c_write_data(uint8_t *data, uint8_t len);
void     i2c_read_data(uint8_t *data, uint8_t len);

#endif
