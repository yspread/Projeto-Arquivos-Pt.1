#include <stdio.h>
#include <stdlib.h>
#include "header.h"

typedef struct header_{
    int status;
    int topo;
    int proxRRN;
    int nroEstacoes;
    int nroParesEstacao;
}HEADER;

HEADER *createHeader()
{
    HEADER *header = (HEADER *)malloc(sizeof(HEADER));//memória alocada dinamicamente para o cabeçalho
    if (header != NULL)
    {
        header->status = 1; //o arquivo está consistente
        header->topo = -1; //nenhum registro localmente removido
        header->proxRRN = 0; //o proximo campo para se adicionar um registro é o campo de RRN 0
        header->nroEstacoes = 0; //nenhuma estação por enquanto
        header->nroParesEstacao = 0; //nenhum par de estações por enquanto
    }
    return header;
}

void deleteHeader(HEADER **header)
{
    free(*header); //libero a memória alocada para o cabeçalho
    *header = NULL;
}

void changeHeaderStatus(HEADER *header) //muda o status do arquivo
{
    if (header == NULL)
    {
        return;
    }
    if (header->status == 1) //se o status for 1, muda pra 0
    {
        header->status = 0;
    }
    else //se o status for 0, muda pra 1
    {
        header->status = 1;
    }
}

void setTopo(HEADER *header, int novotopo)
{
    if(header != NULL)
    {
        header->topo = novotopo;
    }
}

void setProxRRN(HEADER *header, int novoRRN)
{
    if(header != NULL)
    {
        header->proxRRN = novoRRN; //atualiza o valor do campo
    }
}

void setNroEstacoes(HEADER *header, int novovalor)
{
    if (header != NULL)
    {
        header->nroEstacoes = novovalor; //atualiza o valor do campo
    }
}

void setNroParesEstacao(HEADER *header, int novovalor)
{
    if (header != NULL)
    {
        header->nroParesEstacao = novovalor; //atualiza o valor do campo
    }
}


char getStatus(HEADER *header)
{
    if(header != NULL)
    {
        return (header->status);
    }
}

int getTopo(HEADER *header)
{
    if(header != NULL)
    {
        return (header->topo);
    }
}

int getProxRRN(HEADER *header)
{
    if (header != NULL)
    {
        return (header->proxRRN);
    }
}

int getNroEstacoes(HEADER *header)
{
    if (header != NULL)
    {
        return (header->nroEstacoes);
    }
}

int getNroParesEstacao(HEADER *header)
{
    if(header != NULL)
    {
        return (header->nroParesEstacao);
    }
}