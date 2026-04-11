#ifndef REGISTROS_H
    #define REGISTROS_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "header.h"
    
    typedef struct registro_ REGISTRO;
    typedef struct criterios_ CRITERIOS;

    //funções para criar/deletar uma struct de registro
    REGISTRO *createRecord(int codEstacao, int codLinha, int codProxEstacao, int distProxEstacao, int codLinhaIntegra, int codEstIntegra, char* nomeEstacao, char *nomeLinha);
    void deleteRecord(REGISTRO *registro); //apaga um registro da memória
    REGISTRO *recordFromCSV(char *buffer); //cria um registro, retirando os campos de uma string (uma linha lida do arquivo csv) por meio da função strsep
    REGISTRO *recordFromBin(FILE *arqbin, char removido); //le um registro de um arquivo binário. se removido valer 1, a função retorna NULL
    REGISTRO *recordFromInput(); //cria um registro com dados informados pelo usuário
    
    void writeRecordOnBin(REGISTRO *registro, FILE *fp); //escreve todo o conteúdo do registro no arquivo binário e coloca o lixo pra completar os 80 bytes
    void printRecord(REGISTRO *registro); //função para imprimir os dados de um registro
    int recordMeetsCriteria(REGISTRO *registro, int m, CRITERIOS **criterios); //verifica se um registro atende critérios de busca
    
    //funções para criar/deletar uma struct criterios
    CRITERIOS *createCriteria(char *nomeCampo, char *valorCampo); //crio uma struct para criterios de busca
    void deleteCriteria(CRITERIOS *criterios);//libero a memória alocada para uma struct de criterios
    void readCriteria(int m, CRITERIOS **criterios);
    
    //funções para se acessar campos de um registro
    int getCodEstacao(REGISTRO *registro);
    int getCodProxEstacao(REGISTRO *registro);
    char *getNomeEstacao(REGISTRO *registro);

    //funções para se alterar valores de um campo de uma struct de registro
    void setProximo(REGISTRO *registro, int valor);//função para se alterar o valor do campo próximo de um registro
    void removeRecord(REGISTRO *registro); //função para marcar um registro como logicamente removido
    
    //funções para se alterar o valor de um campo de uma struct de criterios
    void setNomeCampo(CRITERIOS *criterios, char *nomeCampo);
    void setValorCampo(CRITERIOS *criterios, char *valorCampo);
    
    void attRecords(REGISTRO *registro, int atts, CRITERIOS **criteriosAtt); //funcao que pega as atualizacoes e escreve no registro
    void attCountersHeader(FILE *arqin, HEADER *header); //funcao para percorrar o arquivo e contar o nroEstacoes e nroParesEstacao

#endif