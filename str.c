#include "str.h"


uint8_t itoa_pad(char *str, int32_t val, uint8_t pad, uint8_t flags)
{
	char *buf = str;
	uint8_t n=0;
	uint8_t sign=val<0;
	if (sign) val = -val;
	// Преобразуем число в строку
	do{
		*str++ = val%10 + '0';
		n ++;
	} while ((val/=10)>0);
	// Знак
	if (sign){
		*str++ = '-';
		n ++;
	}
	// Символ выравнивание
	uint8_t pchar = flags & ITOA_PAD_ZERO ? '0' : ' ';
	// Выравнивание по правой стороне
	while (!(flags & ITOA_PAD_LEFT) && (n<pad)){
		*str++ = pchar;
		n ++;
	}
	// Завершение строки
	if (flags & ITOA_TERMINATE) *str = 0x00;
	// Переворачиваем строку
	str--;
	while (str>buf){
		char c = *str;
		*str-- = *buf;
		*buf++ = c;
	}
	// Выравнивание по левой стороне
	if (flags & ITOA_PAD_LEFT){
		str += n-1;
		while(n<pad){
			*str++ = pchar;
			n ++;
		}
	}
	// Возвращаем длину строки
	return n;
}
