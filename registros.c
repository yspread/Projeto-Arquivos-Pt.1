#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registros.h"

typedef struct registro_{
    //campos de tamanho fixo
    char removido; //indica se registro está logicamente removido
    int proximo; //valor do RRN do proximo registro logicamente removido
    int codEstacao;
    int codLinha;
    int codProxEstacao;
    int distProxEstacao;
    int codLinhaIntegra;
    int codEstIntegra;
    //campos de tamanho variável e seus indicadores de tamanho
    int tamNomeEstacao;
    char *nomeEstacao;
    int tamNomeLinha;
    char *nomeLinha;
}REGISTRO;

//todos os parâmetros serão retirados do arquivo de entrada para se criar um registro
REGISTRO *createRecord(int codEstacao, int codLinha, int codProxEstacao, int distProxEstacao, int codLinhaIntegra, int codEstIntegra, char* nomeEstacao, char *nomeLinha)
{
    REGISTRO *registro = (REGISTRO *)malloc(sizeof(REGISTRO)); //memória alocada dinamicamente para o registro
    if (registro != NULL)
    {
        registro->removido = '0';
        registro->proximo = -1;
        registro->codEstacao = codEstacao;
        registro->codLinha = codLinha;
        registro->codProxEstacao = codProxEstacao;
        registro->distProxEstacao = distProxEstacao;
        registro->codLinhaIntegra = codLinhaIntegra;
        registro->codEstIntegra = codEstIntegra;
        registro->tamNomeEstacao = strlen(nomeEstacao);
        registro->nomeEstacao = nomeEstacao;
        registro->tamNomeLinha = strlen(nomeLinha);
        registro->nomeLinha = nomeLinha;
    }
    return registro;
}

void deleteRecord(REGISTRO **registro)
{
    free(*registro);
    *registro = NULL;
}

REGISTRO *recordFromCSV(char *buffer)
{
    int codEstacao, codLinha, codProxEst, distProxEst, codLinhaIntegra, codEstIntegra;  //todos os tokens que serão obtidos com a strsep
    char *nomeEstacao, *nomeLinha, *tokenTemp;                                          //e serão utilizados na criação de um registro 
    //tokenização de acordo com a ordem dos campos no arquivo csv
    codEstacao = atoi(strsep(&buffer, ",")); //esse valor não pode ser nulo
    nomeEstacao = strsep(&buffer, ",");
    tokenTemp = strsep(&buffer, ",");
    if (tokenTemp == NULL)
    {
        codLinha = atoi(tokenTemp);
    }
    else codLinha = -1;                //no caso dos inteiros, se o campo for nulo, deve ser salvo como -1
    nomeLinha = strsep(&buffer, ","); //se for null, na criação do registro ja considerará tamNomeLinha como 0
    tokenTemp = strsep(&buffer, ",");
    if (tokenTemp == NULL)
    {
        codProxEst = atoi(tokenTemp);
    }
    else codProxEst = -1; 
    tokenTemp = strsep(&buffer, ",");
    if (tokenTemp == NULL)
    {
        distProxEst = atoi(tokenTemp);
    }
    else distProxEst = -1;
    tokenTemp = strsep(&buffer, ",");
    if (tokenTemp == NULL)
    {
        codLinhaIntegra = atoi(tokenTemp);
    }
    else codLinhaIntegra = -1;
    tokenTemp = strsep(&buffer, ",");
    if (tokenTemp == NULL)
    {
        codEstIntegra = atoi(tokenTemp);
    }
    else codEstIntegra = -1;
    //criação do registro usando todos os tokens coletados
    REGISTRO *registro = createRecord(codEstacao, codLinha, codProxEst, distProxEst, codLinhaIntegra, codEstIntegra, nomeEstacao, nomeLinha);
    return registro;
}

REGISTRO *recordFromBin(char *arqbin)
{
    char removido;   
    fread(removido, sizeof(char), 1, arqbin);
    if (removido == '1') //registro está logicamente removido, não deve ser considerado
    {
        fseek(arqbin, 79, SEEK_CUR); //o ponteiro vai para o offset do próximo registro
        return NULL;
    }
    else
    {
        int codEstacao, codLinha, codProxEst, distProxEst, codLinhaIntegra, codEstIntegra, tamNomeEstacao, tamNomeLinha;  //todos os campos que serão lidos do registro
        char *nomeEstacao, *nomeLinha;
        fseek(arqbin, 4, SEEK_CUR); //ponteiro pula o campo registro->proximo
        //leitura de todos os campos do registro
        fread(codEstacao, sizeof(int), 1, arqbin);
        fread(codLinha, sizeof(int), 1, arqbin);
        fread(codProxEst, sizeof(int), 1, arqbin);
        fread(distProxEst, sizeof(int), 1, arqbin);
        fread(codLinhaIntegra, sizeof(int), 1, arqbin);
        fread(codEstIntegra, sizeof(int), 1, arqbin);
        fread(tamNomeEstacao, sizeof(int), 1, arqbin);
        fread(nomeEstacao, tamNomeEstacao, 1, arqbin);
        fread(tamNomeLinha, sizeof(int), 1, arqbin);
        fread(nomeLinha, tamNomeLinha, 1, arqbin);
        int lixo = (80 - tamNomeLinha + tamNomeEstacao + 37); //a quantidade de lixo que deverá ser pulada para colocar o ponteiro do arquivo no offset do próximo registro
        fseek(arqbin, lixo, SEEK_CUR); //ponteiro direcionado para o offset do proximo registro
        REGISTRO *registro = createRecord(codEstacao, codLinha, codProxEst, distProxEst, codLinhaIntegra, codEstIntegra, nomeEstacao, nomeLinha);
        return registro;
    }
}

void writeRecordOnBin(REGISTRO *registro, FILE *fp) //função para se escrever um registro inteiro em um arquivo binário
{
    //escrevo cada um dos campos no arquivo
    fwrite(&registro->removido, sizeof(char), 1,fp); //1 byte
    fwrite(&registro->proximo, sizeof(int), 1,fp); //4 bytes
    fwrite(&registro->codEstacao, sizeof(int), 1,fp); //4 bytes
    fwrite(&registro->codLinha, sizeof(int), 1,fp); //4 bytes
    fwrite(&registro->codProxEstacao, sizeof(int), 1,fp); //4 bytes
    fwrite(&registro->distProxEstacao, sizeof(int), 1,fp); //4 bytes
    fwrite(&registro->codLinhaIntegra, sizeof(int), 1,fp); //4 bytes
    fwrite(&registro->codEstIntegra, sizeof(int), 1,fp); //4 bytes
    fwrite(&registro->tamNomeEstacao, sizeof(int), 1,fp); //4 bytes
    fwrite(&registro->nomeEstacao, registro->tamNomeEstacao, 1,fp); //passando o espaço desejado dessa forma, eu excluo o \0 (que não deve ser salvo)
    fwrite(&registro->tamNomeLinha, sizeof(int), 1,fp); //4 bytes
    fwrite(&registro->tamNomeLinha, registro->tamNomeLinha, 1,fp);
    //temos no total (37 + tamNomeLinha + tamNomeEstacao) bytes, devemos completar o que falta pra 80 bytes com lixo ("$")
    int lixo = 80 - (37 + registro->tamNomeLinha + registro->tamNomeEstacao); 
    for (int i = 0; i < lixo; i++)
    {
        fwrite('$', sizeof(char),1, fp); //escreve todo o lixo necessário até o final do registro
    }
    return;
}

void printRecord(REGISTRO *registro) //essa função vai pegar um registro e imprimir seus campos conforme a formatação pedida para a funcionalidade 2
{
    if (registro == NULL) //se o registro for NULO, nada deve ser impresso
    {
        return;
    }
    printf("%d %s ", registro->codEstacao, registro->nomeEstacao);
    if (registro->codLinha = -1) //para os campos que não existem, deve ser printado como "NULO"
    {
        printf("NULO ");
    }
    else printf("%d ", registro->codLinha);
    if (registro->tamNomeLinha = 0)
    {
        printf("NULO ");
    }
    else printf("%s ", registro->nomeLinha);
    if (registro->codProxEstacao = -1)
    {
        printf("NULO ");
    }
    else printf("%d ", registro->codProxEstacao);
    if (registro->distProxEstacao = -1)
    {
        printf("NULO ");
    }
    else printf("%d ", registro->distProxEstacao);
    if (registro->codLinhaIntegra = -1)
    {
        printf("NULO ");
    }
    else printf("%d ", registro->codLinhaIntegra);
    if (registro->codEstIntegra = -1)
    {
        printf("NULO ");
    }
    else printf("%d\n", registro->codEstIntegra);
}

void searchRecords(int m, char *nomeCampo, char *valorCampo)
{
    for (int i = 0; i < m; i++)
    {
        
    }
}

char *getNomeEstacao(REGISTRO *registro)
{
    if(registro != NULL)
    {
        return (registro->nomeEstacao);
    }
}

int getCodEstacao(REGISTRO *registro)
{
    if(registro != NULL)
    {
        return (registro->codEstacao);
    }
}

int getCodProxEstacao(REGISTRO *registro)
{
    if(registro != NULL)
    {
        return (registro->codProxEstacao);
    }
}