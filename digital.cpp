#include "src/LPC17xx.h"
#include "digital.h"



static LPC_GPIO_TypeDef (* const LPC_GPIO[5]) = { LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3, LPC_GPIO4  };


void pinMode( uint16_t portbit, uint8_t bitVal )
{   
  if (bitVal == OUTPUT)  LPC_GPIO[(uint8_t)(portbit>>5)]->FIODIR |= (1<<(31&portbit));
  else  LPC_GPIO[portbit>>5]->FIODIR &= ~(1<<(31&portbit));
}

void digitalWrite( uint16_t portbit, uint8_t valor )
{
    LPC_GPIO[portbit>>5]->FIOMASK = ~(1<<(31&portbit));
	if (valor == LOW)  LPC_GPIO[(uint8_t)(portbit>>5)]->FIOCLR = (1<<(31&portbit));
    else LPC_GPIO[(uint8_t)(portbit>>5)]->FIOSET = (1<<(31&portbit));
}


uint8_t digitalRead (uint16_t portbit)
{
    uint32_t val;

    LPC_GPIO[portbit>>5]->FIOMASK = ~(1<<(31&portbit));
    val = LPC_GPIO[portbit>>5]->FIOPIN;
    val = val >> (31&portbit);
    LPC_GPIO[portbit>>5]->FIOMASK = 0x00000000;
    return (uint8_t)(val& 1);
}

