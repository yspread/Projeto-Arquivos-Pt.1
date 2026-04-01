#ifndef HEADER_H
    #define HEADER_H
    #include <stdio.h>
    #include <stdlib.h>
    
    typedef struct header_ HEADER;

    HEADER *createHeader(); //função para criar o cabeçalho do arquivo
    void deleteHeader(HEADER **header); //função para apagar o cabeçalho da memória

    //funções para alterar valores de campos da header
    void changeHeaderStatus(HEADER *header); //troca de 0 pra 1 ou de 1 pra 0 o campo status da header
    void setTopo(HEADER *header, int novotopo); //atualiza o valor do topo
    void setProxRRN(HEADER *header, int novoRRN); //atualiza o valor do campo proxRRN da header
    void setNroEstacoes(HEADER *header, int novovalor); //atualiza o valor do campo nroEstacoes da header;
    void setNroParesEstacao(HEADER *header, int novovalor); //atualiza o valor do campo nroParesEstacao da header

    //funções para se acessar campos da header
    char getStatus(HEADER *header); //retorna header->status
    int getTopo(HEADER *header); //retorna o topo da pilha de arquivos logicamente removidos(header->topo)
    int getProxRRN(HEADER *header); //retorna o valor de header->proxRRN
    int getNroEstacoes(HEADER *header); //retorna o valor de header->nroEstacoes
    int getNroParesEstacao(HEADER *header); //retorna o valor header->nroParesEstacao
#endif