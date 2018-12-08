// para compilar:  
//                    make                    ou                        make -f Makefile-linux 
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
	char buffer[70];
	char buffer2[70];
	SA_FILE  *B;

	SA_format();
	B = SA_fopen ("UFSC2","w");
	if (B==NULL)
	{
		printf("Erro tentando criar um arquivo\n");
	}
	else
	{
		printf("Grava dados\n");
		strcpy(buffer, "abcde");

		/*for (int x=0; x< 26; x++) SA_fputc(65+x,B);
		for (int x=0; x< 26; x++) SA_fputc(97+x,B);
		for (int x=0; x< 10; x++) SA_fputc(48+x,B);
		*/
	    meu_fwrite(B,  ( uint8_t*) buffer ,sizeof(char), strlen(buffer));
		meu_fclose(B);

		//int lido;
		printf("Terminou de gravar dados\n");

		
		//B = SA_fopen("UFSC2","r+");
		// mostra as letras minusculas e os numeros
		//SA_fseek(B, 0);
		//printf("SA_fseek\n");
		/*for (int x=0;x<36;x++)
		{
			lido = SA_fgetc(B);
			printf ("%c\n",lido);
		}
		*/

		//printf("%d\n", meu_ftell(B));
		
		meu_fread( B,(uint8_t *) buffer2, sizeof(char), strlen(buffer));
		printf("Lido = %s\n", buffer2);
		meu_fclose(B);

	}
	
	B = SA_fopen ("UFSC2","w");
	if (B==NULL)
	{
		printf("Erro tentando criar um arquivo\n");
	}
	else
	{
		printf("Grava dados\n");
		strcpy(buffer, "fghijk");

	    meu_fwrite(B,  ( uint8_t*) buffer ,sizeof(char), strlen(buffer));
		meu_fclose(B);

		//int lido;
		printf("Terminou de gravar dados\n");

		
		meu_fread( B,(uint8_t *) buffer2, sizeof(char), strlen(buffer));
		printf("Lido = %s\n", buffer2);
		meu_fclose(B);

	}
	
	
    return 0 ;
}
