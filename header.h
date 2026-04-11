#ifndef HEADER_H
    #define HEADER_H
    #include <stdio.h>
    #include <stdlib.h>
    
    typedef struct header_ HEADER;

    HEADER *createHeader(); //função para criar o cabeçalho do arquivo
    void deleteHeader(HEADER *header); //função para apagar o cabeçalho da memória
    HEADER *headerFromBin(FILE *arqbin); //função para ler a header de um arquivo binário
    void writeHeaderOnBin(HEADER *header, FILE *arqbin); //escreve os dados do cabeçalho em um arquivo binário
    void attHeaderOnFile(HEADER *header, FILE *arqbin); //atualiza os campos da header e a escreve no arquivo binário

    //funções para alterar valores de campos da header
    void setStatus(HEADER *header, char novostatus);
    void setTopo(HEADER *header, int novotopo);
    void setProxRRN(HEADER *header, int novoRRN);
    void setNroEstacoes(HEADER *header, int novovalor);
    void setNroParesEstacao(HEADER *header, int novovalor); 

    //funções para se acessar campos da header
    char getStatus(HEADER *header); 
    int getTopo(HEADER *header);
    int getProxRRN(HEADER *header);
    int getNroEstacoes(HEADER *header);
    int getNroParesEstacao(HEADER *header);
#endif