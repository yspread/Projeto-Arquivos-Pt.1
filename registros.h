#ifndef REGISTROS_H
    #define REGISTROS_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    
    typedef struct registro_ REGISTRO;

    //essa função cria um registro utilizando os dados coletados do arquivo de entrada
    REGISTRO *createRecord(int codEstacao, int codLinha, int codProxEstacao, int distProxEstacao, int codLinhaIntegra, int codEstIntegra, char* nomeEstacao, char *nomeLinha);
    void deleteRecord(REGISTRO **registro); //apaga um registro da memória
    
    REGISTRO *recordFromCSV(char *buffer); //cria um registro, retirando os campos de uma string (uma linha lida do arquivo csv) por meio da função strsep
    void writeRecordOnBin(REGISTRO *registro, FILE *fp); //escreve todo o conteúdo do registro no arquivo binário e coloca o lixo pra completar os 80 bytes

    char *getNomeEstacao(REGISTRO *registro);
#endif