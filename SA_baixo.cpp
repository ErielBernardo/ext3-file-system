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
void SA_escreva_bloco_ponteiro(uint16_t numero, uint16_t valor)
{
	/*calcula a posição na memoria 
	do bloco para alocação (ponteiro) (da lista) em questão*/
	uint16_t endereco = numero * 2 + INICIO_AREA_BLOCO_PONTEIROS; 
	memoria_write(endereco, (uint8_t *)&valor, 2);
}

void SA_remove_arquivo (uint8_t id){
	entrada_arquivo tmp;
	
	SA_leia_entrada_arquivo(id, &tmp); // le a entrada desejada e salva no tmp
	
	uint16_t ponteiro_indireto = tmp.indireto;
	if(ponteiro_indireto != 0xFFFF){ //se tiver dados indiretos alocados.
	
		TipoDados  bloco_idx;  //bloco de endereços
		SA_le_bloco_dados (ponteiro_indireto, (uint8_t *) &bloco_idx); //le o bloco do indireto (que tem os endereços dos blocos de dados)
		
		for (int i = 0; i < 16 && bloco_idx.enderecos[i] != 0xffff; i++) // remove os blocos (referenciadas no bloco do indireto) ate achar uma entrada inválida.
		{
			
			SA_escreva_bloco_ponteiro(bloco_idx.enderecos[i], SA_le_cabecalho());
			SA_escreve_cabecalho(bloco_idx.enderecos[i]); // para remover uma entrada, gravamos o valor do cabeçalho no bloco referente a entrada em questão, e marcamos essa como o valor do cabeçalho. (novo.valor = cabecalho.valor; cabecalho.valor = novo; +- assim);
		}
		SA_escreva_bloco_ponteiro(ponteiro_indireto, SA_le_cabecalho());
		SA_escreve_cabecalho(ponteiro_indireto); //faz a mesma coisa, só que agora para o bloco do indireto
	}
	
	cria_entrada(id); //recria a entrada, (zera tudo)
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
			if ((ent.status == 0) && (achou ==-1)) 
			{
				//achou (o bloco livre OTARIO)
				achou = x;
			}
			if (strcmp(nome, ent.nome)==0)
			{
				SA_remove_arquivo(x);
				// arquivo existe, deixa para os alunos fazerem
				/** Se não achar entrada livre
				vamos colcoar o mesmo nome na entrada*/
				if(achou==-1)
				{
					achou =x;
				}
				//break;
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
	else if(strcmp(modo, "r+")==0)
	{
		int livre =-1; /*Se for modo r+*/
		/*procuramos a entrada desejada*/
		for (int x  =0; x< 12; x++)
		{
			SA_leia_entrada_arquivo(x,&ent);
			if (strcmp(ent.status, st)==0)
			{
				livre=x;
				 break;
			}
		}
		/*Fazer retornar NULL se não existir o arquivo*/
		if(livre==-1){
			return NULL;
		}
		/*se existir, retorna o arquivo.*/
		else{
			SA_FILE *tmp;

			tmp = (SA_FILE *) malloc (sizeof(SA_FILE));
			tmp->id = livre;
			tmp->posicao = 0;
		}
	}
	return tmp;
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



/** buffer de bytes a serem escrito
tamanho é o tamanho de cada item, e 
count é a quantidade a ser escrita
**/
uint16_t meu_fwrite(SA_FILE *A, uint8_t * buffer, uint16_t size, uint16_t count) {

    uint16_t qn_esccrito = 0; // é a quantidade de itens que vai ser lido

    // quantidade de bytes que falta escrever
    uint16_t qtd_escrever = size * count;

	//uint8_t P_MEUFILE = A->posicao;//
	uint16_t id = A->id;
	uint16_t posicao = A->posicao;

	uint16_t n;
	/** Para lembrar essa struct eu consigo acessar direto e indireto
	char status;
	char nome[8];
	bloco_dados dados_diretos;
	uint16_t indireto;
	uint16_t tam;
	**/
    entrada_arquivo inodo_lida; 

	TipoDados bloco;
	//TipoDados bloco.bytes;

	if(buffer == NULL || A == NULL)
	{
		//ERRROOO
		return -2; 
	}

	if(count ==0)
	{
		return 0;
	}
	SA_leia_entrada_arquivo(id, &inodo_lida);// le a entrada do arquivo em questão
	


	if (posicao < 32) {
		/*TEM quer fazer um for 
		que a quantidade escrita vai ser menor que a posicao menos 32
		e a quantidade que vai escrever tem que ser maior que zero*/
//printf("teste doido")
		//while((A->posicao+qn_esccrito) <  32 && qtd_escrever > 0) {
		for (; qn_esccrito < (32 - posicao) && qtd_escrever > 0;){	
			// aloca no bloco direto a posicao do buffer
			//printf("@@escrevendo %i @@\n\n ", buffer[qn_esccrito]);
			inodo_lida.dados[A->posicao++] = buffer[qn_esccrito++];
			//qn_esccrito++;
			// decrementa 1 byte
			qtd_escrever--;
			// aumenta tamanho do inodo
			inodo_lida.tamanho++;
		}
		
	}

	/*se for ler ainda e tem q verificar se tem algo no indireto e alocar*/  
	// verifica se chegou no final e tem algo p escrever
	if (inodo_lida.indireto==0xFFFF && qtd_escrever > 0)  {
		/*caso não foi alocado, devemos alocar o indireto*/
		inodo_lida.indireto = aloca ();
	}


	/*Calcula deslocamento e entrada corrente*/
	// dos blocos de endereço seja a inicial
	uint16_t numero_entrada = (A->posicao / 32);
	// calcula o deslocamento caso a poosição 
	uint8_t deslocamento    = A->posicao % 32;
	
	// aloca bloco de dados indireto p começar a operacao
	//SA_le_bloco_dados(uint16_t numero ,Tipo_Bloco *b)

	SA_le_bloco_dados (inodo_lida.indireto, (uint8_t *) &bloco_idx);
	/*Caso a entrada não esteja alocada temos que alocar ela*/
	if (bloco_idx[numero_entrada]==0xffff)
	{
		/*le a entra em questão*/
		bloco_idx[numero_entrada] = aloca();
		/*marca o bloco*/
		
		SA_salva_bloco_dados(inodo_lida.indireto , (uint8_t *) bloco_idx);
	}
	/*le o bloco de dados*/
	SA_le_bloco_dados (inodo_lida.indireto, (uint8_t *) &bloco_idx);
	
	/*bloco da entrada*/
	//enquanto tiver coisa pra escrever e tiver endereço no bloco
	// cuidar o overhead
	// tem que ter o que escrever > 0 e entrada tem que ser menor
	// q o numero de blocos
	
	while(qtd_escrever > 0 && numero_entrada < TAMANHO_BLOCO_INDICES) {
		// se for invalido tem q alocar
		if(bloco_idx[numero_entrada] =0xffff) {
			bloco_idx[numero_entrada] = aloca();
			SA_le_bloco_dados(inodo_lida.indireto , (uint8_t *) bloco_idx); 
			// vai atulizar o ponteiro
		}
		//agr le pq ta atualizado
		SA_le_bloco_dados (inodo_lida.indireto, (uint8_t *) &bloco_idx); 
		n= bloco_idx[numero_entrada];
		SA_le_bloco_dados (n, (uint8_t *) &bloco);//vai ler o  bloco
			// tem que fazer um for pra gravar tipo o do fputc
		// que incrementa o tamnho e faz um deslocamento	
		// nesse for é gravado em um bloco os 
		//valores do vetor passado como parâmetro.
		// int i = deslocamento;
		
	

		for(uint16_t x = deslocamento; x < 32 && qtd_escrever > 0; x++) {
			bloco[x] = buffer[qn_esccrito++];
			qtd_escrever--;
			A->posicao++;
			inodo_lida.tamanho++;
		}

		// escreve novamente o bloco que acabou de ser atualizado(bloco de dados)
		SA_le_bloco_dados(n ,  (uint8_t *) &bloco);
		// zera o deslocamento para o próximo bloco
		deslocamento = 0;
		numero_entrada++;
	}
	//A->posicao = posicao;
	
	SA_salva_entrada_arquivo(id, &inodo_lida);
	return qn_esccrito;
}
	

// Lê um arquivo e passa count bytes para o buffer desejado
uint16_t meu_fread(SA_FILE *A, uint8_t * buffer, uint16_t size, uint16_t count) 
{
    uint16_t qn_lido = 0; // é a quantidade de itens que vai ser lido

    // quantidade de bytes que falta escrever
    uint16_t qtd_ler = size * count;

	//uint8_t P_MEUFILE = A->posicao;//
	uint16_t id = A->id;
	uint16_t posicao = A->posicao;

	uint16_t n;
	
    struct inodo inodo_lida; 

	bloco_dados bloco_idx;
	bloco_dados bloco;

	if(buffer == NULL || A == NULL)
	{
		//ERRROOO
		return -2; 
	}
	if(count ==0)
	{
		return 0;
	}
	SA_leia_entrada_arquivo(id, &inodo_lida);// le a entrada do arquivo em questão
	
	if (posicao < 32) 
	{
		/*TEM quer fazer um for 
		que a quantidade escrita vai ser menor que a posicao menos 32
		e a quantidade que vai escrever tem que ser maior que zero*/
		
		for (; A->posicao < 32 && qtd_ler > 0 && A->posicao <= inodo_lida.tamanho;){
				buffer[qn_lido++] = inodo_lida.dados_diretos[A->posicao++]; //lemos dos dados diretos até não ter mais espaço, ainda houver algo para ler (se não chegar no fim do arquivo) e enquanto a qtd a ser lida não foi lida
				qtd_ler--;
			}
	
	}

	/*se for ler ainda e tem q verificar se tem algo no indireto e alocar*/  
	// verifica se chegou no final e tem algo p escrever
	if (inodo_lida.indireto==0xFFFF && qtd_ler > 0)  
	{
		/*caso não foi alocado, devemos alocar o indireto*/
		inodo_lida.indireto = aloca ();
	}


	/*Calcula deslocamento e entrada corrente*/
	// dos blocos de endereço seja a inicial
	uint16_t numero_entrada = (A->posicao / 32);
	// calcula o deslocamento caso a poosição 
	uint8_t deslocamento    = A->posicao % 32;
	
	// aloca bloco de dados indireto p começar a operacao
	
	SA_le_bloco_dados (inodo_lida.indireto, (uint8_t *) &bloco_idx);
	/*Caso a entrada não esteja alocada temos que alocar ela*/
	while(qtd_ler > 0 && A->posicao <= inodo_lida.tamanho && numero_entrada < TAMANHO_BLOCO_INDICES)
	{ //enquanto se quer ler algo, o final do arquivo nao chegar e o numeor da entrada (do bloco de endereços) for menor do que o tamanho (16 entradas são possíveis)
				uint16_t n = bloco_idx[numero_entrada]; 
				SA_le_bloco_dados (n, (uint8_t *) &bloco); //le o proximo bloco
				for (int x = deslocamento; x < 32 && qtd_ler > 0 && A->posicao <= inodo_lida.tamanho; x++)
				{
					buffer[qn_lido++] = bloco[x]; //le cada posição do bloco em questão
					qtd_ler--;
					A->posicao++;
				}
				deslocamento = 0;
				numero_entrada++;
	}
		
	buffer[size * count] = '\0'; //grava a pŕoxima posição como caracter final de frase. (caso for uma string)
	return qn_lido; //retorna quantos bytes foram lidos
	

}


int meu_fclose(MEU_FILE *A)
{
	if(A == NULL){
		return 1;
	}
	/*Acho que funciona faz uns testes ai*/
	free(A);
	return 0;

}

uint16_t meu_ftell(SA_FILE *A) //RETORNA 0xFFFF se A for Nulo (arquivo estiver fechado)
{
	if (A != NULL)
		return A->posicao; //ftell retorna a posição corrente no arquivo A e 0xFFFF se o arquivo for nulo.
	else{
		return 0xFFFF; 
	}	
}


int meu_feof (SA_FILE *A)
{

	/*Vai retornar zero  se a corrente for menor q o
	tamanho e retorna 1 se a posição for 
	maior ou igual(chegou no final do arquivo.*/
	entrada_arquivo lida;
	if (A == NULL) return 1;
	uint16_t id = A->id;

	
		
	SA_leia_entrada_arquivo(id, &lida);
	
	if (lida.tamanho > A->posicao) return 0;
	return 1;
	
}