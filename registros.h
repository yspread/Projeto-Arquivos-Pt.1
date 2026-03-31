#ifndef REGISTROS_H
    #define REGISTROS_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    
    typedef struct registro_ REGISTRO;

    //essa função cria um registro utilizando os dados coletados do arquivo de entrada
    REGISTRO *createRegister(int codEstacao, int codLinha, int codProxEstacao, int distProxEstacao, int codLinhaIntegra, int codEstIntegra, char* nomeEstacao, char *nomeLinha);
    void deleteRegister(REGISTRO **registro); //apaga um registro da memória
    REGISTRO *registerFromCSV(char *buffer); //cria um registro, retirando os campos de uma string (uma linha lida do arquivo csv) por meio da função strsep
    void writeRegisterOnFile(REGISTRO *registro, FILE *fp); //escreve todo o conteúdo do registro no arquivo e coloca o lixo pra completar os 80 bytes
#endif