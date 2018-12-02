// para compilar:  
//                    make
// para gravar na placa (usando o bootloader): 
//                    make flash

#include "memoria.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "SA_baixo.h"

#ifdef _USA_ARM_

#include "src/LPC17xx.h"
#include "uart.h"

#endif

int main ( void ) 
{
	
	
#ifdef _USA_ARM_
	SystemInit();
	UART0_Init(9600);
	printf("Rodando\n");
#endif
	memoria_init();

	SA_FILE  *B;

	SA_format();
	B = SA_fopen ("UFSC","w");
	if (B==NULL)
	{
		printf("Erro tentando criar um arquivo\n");
	}
	else
	{
		printf("Grava dados\n");

		for (int x=0; x< 26; x++) SA_fputc(65+x,B);
		for (int x=0; x< 26; x++) SA_fputc(97+x,B);
		for (int x=0; x< 10; x++) SA_fputc(48+x,B);
		
			
		int lido;
		printf("Terminou de gravar dados\n");

		
	
		// mostra as letras minusculas e os numeros
		SA_fseek(B, 26);
		for (int x=0;x<36;x++)
		{
			lido = SA_fgetc(B);
			printf ("%c\n",lido);
		}
		
			}
	
	
    return 0 ;
}

