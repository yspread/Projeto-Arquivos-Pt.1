#ifndef HEADER_H
    #define HEADER_H
    #include <stdio.h>
    #include <stdlib.h>
    
    typedef struct header_ HEADER;

    HEADER *createHeader(); //função para criar o cabeçalho do arquivo
    void deleteHeader(HEADER **header); //função para apagar o cabeçalho da memória
    void changeHeaderStatus(HEADER *header); //altera o status do arquivo
    void setNextRRN(HEADER *header, int novoRRN); //atualiza o valor do campo proxRRN da header
    int getProxRRN(HEADER *header); //retorna o valor de header->proxRRN
#endif