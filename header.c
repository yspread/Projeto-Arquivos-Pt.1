#include <stdio.h>
#include <stdlib.h>
#include "header.h"

typedef struct header_{
    char status;
    int topo;
    int proxRRN;
    int nroestacoes;
    int nroparesestacao;
}HEADER;

//aloca espaço na memória para se armazenar as informações do cabeçalho de um arquivo binário
HEADER *createHeader()
{
    HEADER *header = (HEADER *)malloc(sizeof(HEADER)); //memória alocada dinamicamente para o cabeçalho
    if (header != NULL)
    {
        header->status = '1'; //o arquivo está consistente
        header->topo = -1; //nenhum registro localmente removido
        header->proxRRN = 0; //o proximo campo para se adicionar um registro (sem considerar registros logicamente removidos) é o campo de RRN 0
        header->nroestacoes = 0; //nenhuma estação por enquanto
        header->nroparesestacao = 0; //nenhum par de estações por enquanto
    }
    return header;
}

//função para se liberar o espaço alocado para uma header
void deleteHeader(HEADER *header)
{
    free(header); //libero a memória alocada para o cabeçalho
    header = NULL;
}

//escreve os campos da header em um arquivo binário
void writeHeaderOnBin(HEADER *header, FILE *arqbin)
{
    char status = getStatus(header);
    int topo = getTopo(header);
    int proxRRN = getProxRRN(header);
    int nroEstacoes = getNroEstacoes(header);
    int nroParesEstacao = getNroParesEstacao(header);
    fwrite(&status, sizeof(char), 1, arqbin);
    fwrite(&topo, sizeof(int), 1, arqbin);
    fwrite(&proxRRN, sizeof(int), 1, arqbin);
    fwrite(&nroEstacoes, sizeof(int), 1, arqbin);
    fwrite(&nroParesEstacao, sizeof(int), 1, arqbin);
}

//muda o status do arquivo
void changeHeaderStatus(HEADER *header)
{
    if (header == NULL)
    {
        return;
    }
    if (header->status == '1') //se o status for 1, muda pra 0
    {
        header->status = '0';
    }
    else //se o status for 0, muda pra 1
    {
        header->status = '1';
    }
}

//atualiza o valor do campo topo da header
void setTopo(HEADER *header, int novotopo)
{
    if(header != NULL)
    {
        header->topo = novotopo;
    }
}

//atualiza o valor do campo proxRRN da header
void setProxRRN(HEADER *header, int novoRRN)
{
    if(header != NULL)
    {
        header->proxRRN = novoRRN;
    }
}

//atualiza o valor do campo topo da header
void setNroEstacoes(HEADER *header, int novovalor)
{
    if (header != NULL)
    {
        header->nroestacoes = novovalor;
    }
}

//atualiza o valor do paresestacao topo da header
void setNroParesEstacao(HEADER *header, int novovalor)
{
    if (header != NULL)
    {
        header->nroparesestacao = novovalor;
    }
}

//acessa o campo status da header
char getStatus(HEADER *header)
{
    if(header != NULL)
    {
        return (header->status);
    }
}

//acessa o campo topo da header
int getTopo(HEADER *header)
{
    if(header != NULL)
    {
        return (header->topo);
    }
}

//acessa o campo proxRRN da header
int getProxRRN(HEADER *header)
{
    if (header != NULL)
    {
        return (header->proxRRN);
    }
}

//acessa o campo nroestacoes da header
int getNroEstacoes(HEADER *header)
{
    if (header != NULL)
    {
        return (header->nroestacoes);
    }
}

//acessa o campo nroparesestacao
int getNroParesEstacao(HEADER *header)
{
    if(header != NULL)
    {
        return (header->nroparesestacao);
    }
}