#ifndef _DELAY_
#define _DELAY_

#include <stdint.h>

#define ST_CTRL     (*((volatile unsigned long*)0xE000E010)) //page 783

void delay_ms(uint32_t n);
void delay_us(uint32_t n);


#endif
