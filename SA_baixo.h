
#ifndef _SA_
#define _SA_
#pragma pack(1)

#include <inttypes.h> 

// Definicoes do SA e da EEPROM
#define MAXIMO_ARQUIVOS 12
#define TAMANHO_EEPROM 8*1024
#define TAMANHO_BLOCO_PONTEIRO 2

// inicio de cada area de memoria
#define INICIO_AREA_ENTRADA_ARQUIVOS   (TAMANHO_BLOCO_PONTEIRO)
#define INICIO_AREA_BLOCO_PONTEIROS    (CABECALHO)


// tamanhos de blocos
#define TAMANHO_BLOCO 34
#define CABECALHO (TAMANHO_BLOCO_PONTEIRO+MAXIMO_ARQUIVOS*sizeof(entrada_arquivo))


#define QTD_BLOCOS ((TAMANHO_EEPROM-CABECALHO)/TAMANHO_BLOCO)

#define BLOCO(x) (INICIO_AREA_BLOCO_PONTEIROS+x*TAMANHO_BLOCO)

#define INVALIDO 0xFFFF


typedef struct {
	uint16_t indireto;
	uint16_t tamanho;
	uint8_t  status;
	char     nome[8];
	char     dados[32];
}entrada_arquivo;

typedef struct {
		int id;
		int posicao;


}SA_FILE;

typedef union {
		uint16_t  enderecos[16];
		char      bytes[32];
}TipoDados;

typedef struct {
	uint16_t   ponteiro;
	TipoDados  dados;
}Tipo_Bloco;

int SA_feof(SA_FILE *a);

void SA_format(void);
SA_FILE * SA_fopen (const char *nome, const char *modo);
void SA_fputc(uint8_t valor,SA_FILE *a);
void SA_fseek(SA_FILE *a, uint16_t posicao);
int SA_fgetc(SA_FILE *a);


#endif
