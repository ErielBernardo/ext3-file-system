#include "memoria.h"
#include "SA_baixo.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


uint16_t SA_le_cabecalho (void)
{
	uint16_t bloco;
	memoria_read (0, 0, (uint8_t *) &bloco, sizeof(uint16_t));
	return bloco;
}

void SA_escreve_cabecalho (uint16_t bloco)
{
	memoria_write (0, 0, (uint8_t *) &bloco, sizeof(uint16_t));
}
void SA_salva_entrada_arquivo(uint16_t numero, entrada_arquivo e)
{
	uint16_t posicao = 2 + (numero * sizeof(entrada_arquivo));
	memoria_write (0, posicao, (uint8_t *) &e, sizeof(entrada_arquivo));
}
void SA_leia_entrada_arquivo (uint16_t numero, entrada_arquivo *e)
{
		uint16_t posicao = 2 + (numero * sizeof(entrada_arquivo));
	memoria_read (0, posicao, (uint8_t *) e, sizeof(entrada_arquivo));


}
void SA_salva_bloco_dados(uint16_t numero ,Tipo_Bloco b)
{
	uint16_t posicao = INICIO_AREA_BLOCO_PONTEIROS+ numero*TAMANHO_BLOCO;
	memoria_write (0, posicao, (uint8_t *) &b, sizeof(Tipo_Bloco));
}

void SA_le_bloco_dados(uint16_t numero ,Tipo_Bloco *b)
{
	uint16_t posicao = INICIO_AREA_BLOCO_PONTEIROS+ numero*TAMANHO_BLOCO;
	memoria_read (0, posicao, (uint8_t *) b, sizeof(Tipo_Bloco));
}
SA_FILE * SA_fopen (const char *nome, const char *modo)
{
	entrada_arquivo ent;

	if (strcmp (modo, "w")==0)
	{

		int achou = -1;
		for (int x=0;x < 12;x++) 
		{
			SA_leia_entrada_arquivo(x, &ent);
			if ((ent.status == 0) && (achou ==-1)) achou = x;
			if (strcmp(nome, ent.nome)==0)
			{
				
				// arquivo existe, deixa para os alunos fazerem
				break;
			}

		}
		// arquivo nao existe, cria
		if (achou != -1)
		{
			// existe vaga
			ent.status = 1;
			strcpy(ent.nome,nome);
			ent.tamanho = 0;
			SA_salva_entrada_arquivo(achou, ent);
			// aloca o file e retorna ele
			SA_FILE *tmp;
			tmp = (SA_FILE *) malloc (sizeof(SA_FILE));
			tmp->id = achou;
			tmp->posicao = 0;
			return tmp;
		}
		else return NULL; 

		

	}
	return NULL;
}
uint16_t aloca (void)
{	
	Tipo_Bloco b;
	uint16_t bloco = SA_le_cabecalho();
	SA_le_bloco_dados (bloco, &b);
	SA_escreve_cabecalho(b.ponteiro);

	for (int x=0; x< 16;x++) b.dados.enderecos[x] = INVALIDO;
	SA_salva_bloco_dados (bloco, b);
	return bloco;
}
void SA_fputc(uint8_t valor,SA_FILE *a)
{
	int16_t id = a->id;  uint16_t bloco, deslocamento;
	int16_t posicao = a->posicao;
	entrada_arquivo algumaCoisa;
	SA_leia_entrada_arquivo (id, &algumaCoisa);

	bloco = posicao / 32;
	deslocamento = posicao % 32;


	if (bloco == 0)
	{
		algumaCoisa.dados[deslocamento] = valor;
		algumaCoisa.tamanho++;
		SA_salva_entrada_arquivo (id, algumaCoisa);
		a->posicao  =  a->posicao + 1;

	}
	else
	{
		if (algumaCoisa.indireto == INVALIDO)
		{
			algumaCoisa.indireto = aloca ();
			SA_salva_entrada_arquivo (id, algumaCoisa);
		}
		Tipo_Bloco blocoIndices; 
		Tipo_Bloco blocoDados;

		SA_le_bloco_dados(algumaCoisa.indireto, &blocoIndices);

		if (blocoIndices.dados.enderecos[bloco-1] == INVALIDO) 
		{
			blocoIndices.dados.enderecos[bloco-1] = aloca();
			SA_salva_bloco_dados(algumaCoisa.indireto, blocoIndices);

		}

		SA_le_bloco_dados(blocoIndices.dados.enderecos[bloco-1], &blocoDados);
		blocoDados.dados.bytes[deslocamento] = valor;
		SA_salva_bloco_dados(blocoIndices.dados.enderecos[bloco-1], blocoDados);
algumaCoisa.tamanho++;
a->posicao  =  a->posicao + 1;
SA_salva_entrada_arquivo (id, algumaCoisa);

	}


}


int SA_fgetc(SA_FILE *a)
{
	int16_t id = a->id;  uint16_t bloco, deslocamento;
	int16_t posicao = a->posicao;
	entrada_arquivo algumaCoisa;
	SA_leia_entrada_arquivo (id, &algumaCoisa);

	bloco = posicao / 32;
	deslocamento = posicao % 32;


	if (bloco == 0)
	{
		a->posicao  =  a->posicao + 1;
		return algumaCoisa.dados[deslocamento];
	}
	else
	{
		if (algumaCoisa.indireto == INVALIDO)
		{
			return -1;
		}
		Tipo_Bloco blocoIndices; 
		Tipo_Bloco blocoDados;

		SA_le_bloco_dados(algumaCoisa.indireto, &blocoIndices);

		if (blocoIndices.dados.enderecos[bloco-1] == INVALIDO) 
		{
			return -1;
		}

		SA_le_bloco_dados(blocoIndices.dados.enderecos[bloco-1], &blocoDados);
		a->posicao  =  a->posicao + 1;
		return blocoDados.dados.bytes[deslocamento];

	}


}
int SA_feof(SA_FILE *a)
{
	entrada_arquivo algumaCoisa;
	SA_leia_entrada_arquivo (a->id, &algumaCoisa);
	if (a->posicao == algumaCoisa.tamanho) return 1;
	return 0;
}
void SA_fseek(SA_FILE *a, uint16_t posicao)
{
	a->posicao = posicao;
}

void SA_format(void)
{
	printf("SA_format\n");
	SA_escreve_cabecalho(0);

	entrada_arquivo ent;
	Tipo_Bloco bloco;

	ent.status = 0;
	strcpy(ent.nome,"vazio");
	ent.indireto=INVALIDO;
	ent.tamanho=0;

	for (int x=0;x<32;x++) ent.dados[x]=0;
	for (int x=0;x<MAXIMO_ARQUIVOS;x++) 	SA_salva_entrada_arquivo(x, ent);

	for (int x=0;x<32;x++) bloco.dados.bytes[x]=0;
	for (uint16_t x=0;x<QTD_BLOCOS-1;x++)
	{
		bloco.ponteiro=x+1;
		SA_salva_bloco_dados(x,bloco);
	}
	bloco.ponteiro=INVALIDO;
	SA_salva_bloco_dados(QTD_BLOCOS-1,bloco);




}
