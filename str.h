#ifndef _STR_H
#define _STR_H

#include <stdint.h>


#define ITOA_TERMINATE  0x01
#define ITOA_SIGN       0x02
#define ITOA_PAD_ZERO   0x04
#define ITOA_PAD_LEFT   0x08


uint8_t itoa_pad(char *str, int32_t val, uint8_t pad, uint8_t flags);

#endif
