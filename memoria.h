
#include "i2c_software.h"

void memoria_init (void);

uint16_t memoria_write (uint8_t dispositivo, uint16_t end, uint8_t b[], uint16_t qtd);

uint16_t memoria_read (uint8_t dispositivo, uint16_t end, uint8_t b[], uint16_t qtd);
