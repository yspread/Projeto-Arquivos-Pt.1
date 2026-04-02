#ifndef FUNCIONALIDADES_H
    #define FUNCIONALIDADES_H

    void readRecords(char *arqentrada, char *arqsaida); //função que recebe pega os registros do arquivo de nome arqentrada e passa tudo para o arquivo binário arqsaida
    void showRecords(char *arqentrada); //função para imprimir as informações do arquivo de entrada, desconsiderando registros logicamente removidos
    void filterRecords(char *arqentrada, int n); //função para imprimir as informações do arquivo de entrada porém com um filtro por campo
    void removeRecords(char *arqentrada, int n); //remove logicamente registros do arquivo de entrada
    void insertRecords(); //insere registros no arquivo de entrada
    void updateRecords(); //atualiza registros no arquivo de entrada
#endif