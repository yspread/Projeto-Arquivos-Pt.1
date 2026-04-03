#ifndef REGISTROS_H
    #define REGISTROS_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    
    typedef struct registro_ REGISTRO;
    typedef struct criterios_ CRITERIOS;

    //essa função cria um registro utilizando os dados coletados do arquivo de entrada
    REGISTRO *createRecord(int codEstacao, int codLinha, int codProxEstacao, int distProxEstacao, int codLinhaIntegra, int codEstIntegra, char* nomeEstacao, char *nomeLinha);
    void deleteRecord(REGISTRO *registro); //apaga um registro da memória
    CRITERIOS *createCriteria(char *nomeCampo, char *valorCampo);
    void deleteCriteria(CRITERIOS *criterios);
    
    REGISTRO *recordFromCSV(char *buffer); //cria um registro, retirando os campos de uma string (uma linha lida do arquivo csv) por meio da função strsep
    REGISTRO *recordFromBin(FILE *arqbin); //le 80 bytes do arquivo binário e cria um registro com os dados recolhidos (a função verifica se o registro está logicamente removido ou não, caso sim, retorna NULL)
    void writeRecordOnBin(REGISTRO *registro, FILE *fp); //escreve todo o conteúdo do registro no arquivo binário e coloca o lixo pra completar os 80 bytes
    void printRecord(REGISTRO *registro); //função para imprimir os dados de um registro
    int recordMeetsCriteria(REGISTRO *registro, int m, CRITERIOS **criterios);

    int getCodEstacao(REGISTRO *registro);
    int getCodProxEstacao(REGISTRO *registro);
    char *getNomeEstacao(REGISTRO *registro);

    void setNomeCampo(CRITERIOS *criterios, char *nomeCampo);
    void setValorCampo(CRITERIOS *criterios, char *valorCampo);
    void escreverNoRegistro(FILE *arqin, char *codEstacao, char *codLinha, char *codProxEstacao, char *distProxEstacao, char *codLinhaIntegra, char *codEstIntegra, char *nomeEstacao, char *nomeLinha); //esta funcao vai pegar os dados do registro e inserir no arquivo (funcionalidade 5, de insercao)
#endif