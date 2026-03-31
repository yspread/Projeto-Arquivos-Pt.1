#ifndef FUNCIONALIDADES_H
    #define FUNCIONALIDADES_H

    void readRegisters(char *arqentrada, char *arqsaida); //função que recebe pega os registros do arquivo de nome arqentrada e passa tudo para o arquivo binário arqsaida
    void showRegisters(char *arqentrada); //função para imprimir as informações do arquivo de entrada, desconsiderando registros logicamente removidos
    void filterRegisters(); //função para imprimir as informações do arquivo de entrada porém com um filtro por campo
    void removeRegisters(); //remove logicamente registros do arquivo de entrada
    void insertRegisters(); //insere registros no arquivo de entrada
    void updateRegisters(); //atualiza registros no arquivo de entrada
#endif