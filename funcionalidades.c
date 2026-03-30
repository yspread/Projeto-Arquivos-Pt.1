#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcionalidades.h"
#include "header.h"
#include "registros.h"
#include "fornecidas.h"

/*essa função abrirá o arquivo "arqentrada", receberá os registros presentes nele
e escreverá o cabeçalho e todos os registros de modo binário no arquivo "arqsaida"
Para isso, vamos ler e escrever um registro por vez
Inicialmente um cabeçalho placeholder será escrito no binário mas posteriormente ele será
substituido por outro com os dados finais e corretos*/
void readRegisters(char *arqentrada, char *arqsaida){
    HEADER *header = createHeader(); //crio uma header que será preenchida conforme os registros são lidos
    REGISTRO *registrotemp; //esse registro temporário armazenará o registro que vai ser escrito no binário
    char buffer[100]; //armazenará a linha que foi lida do csv
    FILE *arqin = fopen(arqentrada, "r"); //abertura do arquivo de entrada para leitura
    if (arqin == NULL)
    {
        printf("Não foi possível abrir o arquivo \"%s\"", arqentrada);
    }
    FILE *arqout = fopen(arqsaida, "wb"); //abertura do arquivo de saida para escrita em binário
    changeHeaderStatus(header); //o arquivo foi aberto para escrita, status atualizado
    fwrite(&header, sizeof(header), 1, arqout); //escrita do cabeçalho inicial que será posteriormente sobrescrito
    fgets(buffer, 100, arqin); //primeira linha deve ser ignorada
    while (fgets(buffer, 100, arqin) != NULL) //enquanto não acaba o arquivo
    {
        registrotemp = registerFromString(buffer); //crio registro temporário com os valores presentes na linha
        writeRegisterOnFile(registrotemp, arqout); //escrevo os 80 bytes do registro no arquivo binário
        setNextRRN(header, (getProxRRN(header)+1));
        /*AGORA VEM A MERDA
        como caralhos lidar com o nroParesEstacoes diferentes
        e com o nome de estacoes diferentes*/
    }
}
