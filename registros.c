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

//essa struct vai armazenar os critérios que devem ser cumpridos por um registro durante as buscas
typedef struct criterios_{
    char *nomeCampo;
    char *valorCampo;
}CRITERIOS;

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

void deleteRecord(REGISTRO *registro)
{
    free(registro);
    registro = NULL;
}

REGISTRO *recordFromCSV(char *buffer)
{
    int codEstacao, codLinha, codProxEst, distProxEst, codLinhaIntegra, codEstIntegra;  //todos os tokens que serão obtidos com a strsep
    char *nomeEstacao, *nomeLinha, *tokenTemp;                                          //e serão utilizados na criação de um registro

    buffer[strcspn(buffer, "\r\n")] = '\0'; //limpa possíveis \r e \n que possam ter sido lidos
    //tokenização de acordo com a ordem dos campos no arquivo csv
    codEstacao = atoi(strsep(&buffer, ",")); //esse valor não pode ser nulo
    nomeEstacao = strsep(&buffer, ",");
    tokenTemp = strsep(&buffer, ",");
    if (tokenTemp[0] != '\0')
    {
        codLinha = atoi(tokenTemp);
    }
    else codLinha = -1;                //no caso dos inteiros, se o campo for nulo, deve ser salvo como -1
    nomeLinha = strsep(&buffer, ","); //se for null, na criação do registro ja considerará tamNomeLinha como 0
    tokenTemp = strsep(&buffer, ",");
    if (tokenTemp[0] != '\0')
    {
        codProxEst = atoi(tokenTemp);
    }
    else codProxEst = -1; 
    tokenTemp = strsep(&buffer, ",");
    if (tokenTemp[0] != '\0')
    {
        distProxEst = atoi(tokenTemp);
    }
    else distProxEst = -1;
    tokenTemp = strsep(&buffer, ",");
    if (tokenTemp[0] != '\0')
    {
        codLinhaIntegra = atoi(tokenTemp);
    }
    else codLinhaIntegra = -1;
    tokenTemp = strsep(&buffer, ",");
    if (tokenTemp[0] != '\0')
    {
        codEstIntegra = atoi(tokenTemp);
    }
    else codEstIntegra = -1;
    //criação do registro usando todos os tokens coletados
    REGISTRO *registro = createRecord(codEstacao, codLinha, codProxEst, distProxEst, codLinhaIntegra, codEstIntegra, nomeEstacao, nomeLinha);
    return registro;
}

REGISTRO *recordFromBin(FILE *arqbin)
{
    char removido;   
    fread(&removido, sizeof(char), 1, arqbin);
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
        fread(&codEstacao, sizeof(int), 1, arqbin);
        fread(&codLinha, sizeof(int), 1, arqbin);
        fread(&codProxEst, sizeof(int), 1, arqbin);
        fread(&distProxEst, sizeof(int), 1, arqbin);
        fread(&codLinhaIntegra, sizeof(int), 1, arqbin);
        fread(&codEstIntegra, sizeof(int), 1, arqbin);
        fread(&tamNomeEstacao, sizeof(int), 1, arqbin);
        fread(nomeEstacao, tamNomeEstacao, 1, arqbin);
        fread(&tamNomeLinha, sizeof(int), 1, arqbin);
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
    if (registro->tamNomeEstacao != 0)
    {
        fwrite(registro->nomeEstacao, registro->tamNomeEstacao, 1,fp); //passando o espaço desejado dessa forma, eu excluo o \0 (que não deve ser salvo)
    }
    fwrite(&registro->tamNomeLinha, sizeof(int), 1,fp); //4 bytes
    if (registro->tamNomeLinha != 0)
    {
        fwrite(registro->nomeLinha, registro->tamNomeLinha, 1,fp);
    }
    //temos no total (37 + tamNomeLinha + tamNomeEstacao) bytes, devemos completar o que falta pra 80 bytes com lixo ("$")
    int qtdlixo = 80 - (37 + registro->tamNomeLinha + registro->tamNomeEstacao); 
    char lixo = '$';
    for (int i = 0; i < qtdlixo; i++)
    {
        fwrite(&lixo, sizeof(char),1, fp); //escreve todo o lixo necessário até o final do registro
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
    if (registro->codLinha == -1) //para os campos que não existem, deve ser printado como "NULO"
    {
        printf("NULO ");
    }
    else printf("%d ", registro->codLinha);
    if (registro->tamNomeLinha == 0)
    {
        printf("NULO ");
    }
    else printf("%s ", registro->nomeLinha);
    if (registro->codProxEstacao == -1)
    {
        printf("NULO ");
    }
    else printf("%d ", registro->codProxEstacao);
    if (registro->distProxEstacao == -1)
    {
        printf("NULO ");
    }
    else printf("%d ", registro->distProxEstacao);
    if (registro->codLinhaIntegra == -1)
    {
        printf("NULO ");
    }
    else printf("%d ", registro->codLinhaIntegra);
    if (registro->codEstIntegra == -1)
    {
        printf("NULO\n");
    }
    else printf("%d\n", registro->codEstIntegra);
}

int recordMeetsCriteria(REGISTRO *registro, int m, CRITERIOS **criterios)
{
    for (int i = 0; i < m; i++)
    {
        if (strcmp(criterios[i]->nomeCampo, "codEstacao"))
        {
            if(registro->codEstacao != atoi(criterios[i]->valorCampo))
            {
                return 0;
            }
        }
        if (strcmp(criterios[i]->nomeCampo, "codLinha"))
        {
            if(criterios[i]->valorCampo == "NULO")
            {
                if(registro->codLinha != -1)
                {
                    return 0;
                }
            }
            else
            {
                if(registro->codLinha != atoi(criterios[i]->valorCampo))
                {
                    return 0;
                }
            }
        }
        if (strcmp(criterios[i]->nomeCampo, "codProxEstacao"))
        {
            if(criterios[i]->valorCampo == "NULO")
            {
                if(registro->codProxEstacao != -1)
                {
                    return 0;
                }
            }
            else
            {
                if(registro->codProxEstacao != atoi(criterios[i]->valorCampo))
                {
                    return 0;
                }
            }
        }
        if (strcmp(criterios[i]->nomeCampo, "distProxEstacao"))
        {
            if(criterios[i]->valorCampo == "NULO")
            {
                if(registro->distProxEstacao != -1)
                {
                    return 0;
                }
            }
            else
            {
                if(registro->distProxEstacao != atoi(criterios[i]->valorCampo))
                {
                    return 0;
                }
            }
        }
        if (strcmp(criterios[i]->nomeCampo, "codLinhaIntegra"))
        {
            if(criterios[i]->valorCampo == "NULO")
            {
                if(registro->codLinhaIntegra != -1)
                {
                    return 0;
                }
            }
            else
            {
                if(registro->codLinhaIntegra != atoi(criterios[i]->valorCampo))
                {
                    return 0;
                }
            }
        }
        if (strcmp(criterios[i]->nomeCampo, "codEstIntegra"))
        {
            if(criterios[i]->valorCampo == "NULO")
            {
                if(registro->codEstIntegra != -1)
                {
                    return 0;
                }
            }
            else
            {
                if(registro->codEstIntegra != atoi(criterios[i]->valorCampo))
                {
                    return 0;
                }
            }
        }
        if (strcmp(criterios[i]->nomeCampo, "nomeEstacao"))
        {
            if(registro->nomeEstacao != criterios[i]->valorCampo)
            {
                return 0;
            }
        }
        if (strcmp(criterios[i]->nomeCampo, "nomeLinha"))
        {
            if(criterios[i]->valorCampo == "NULO")
            {
                if(registro->tamNomeLinha != 0)
                {
                    return 0;
                }
            }
            else
            {
                if(registro->nomeLinha != criterios[i]->valorCampo)
                {
                    return 0;
                }
            }
        }
    }
    return 1; //o registro atendeu a todos os critérios
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

//depois ver a alocação de memória dessas porras
void setNomeCampo(CRITERIOS *criterios, char *nomeCampo)
{
    criterios->nomeCampo = nomeCampo;
}

void setValorCampo(CRITERIOS *criterios, char *valorCampo)
{
    criterios->valorCampo = valorCampo;
}

void escreverNoRegistro(FILE *arqin, char *codEstacao, char *codLinha, char *codProxEstacao, char *distProxEstacao, char *codLinhaIntegra, char *codEstIntegra, char *nomeEstacao, char *nomeLinha) {
    char zero ='0';
    int menosUm = -1;
    int tamNomeEstacao;
    int tamNomeLinha;
    int contBytes =0; //vamos usar essas variavel para contar os bytes (do max de 80) do registro. no fim, o restante nao preenchido
                     // sera preenchido com $
    fwrite(&zero, sizeof(char), 1, arqin); //como o primeiro campo indica remocao, é inicializado com 0
    contBytes++; //como é um char, avançamos 1 byte na contagem
    fwrite(&menosUm, sizeof(int), 1, arqin); //o segundo campo deve ser incializado com -1
    contBytes += 4; //como esse campo é um int, avançamos 4 bytes
    if (strcmp(codEstacao, "NULO") == 0) { //se o codEstacao for NULO, marcamos o campo como -1
        fwrite(&menosUm, sizeof(int),1,arqin);
    }
    else { //se nao for nulo, marcamos como o input lido
        int valorEmInt = atoi(codEstacao); //usamos isso para transformar o numero lido como string em int
        fwrite(&valorEmInt, sizeof(int), 1, arqin);
    }

    if (strcmp(codLinha, "NULO") == 0) { //se o codLinha for NULO, marcamos o campo como -1
        fwrite(&menosUm, sizeof(int),1,arqin);
    }
    else { //se nao for nulo, marcamos como o input lido
        int valorEmInt = atoi(codLinha); //usamos isso para transformar o numero lido como string em int
        fwrite(&valorEmInt, sizeof(int), 1, arqin);
    }

    if (strcmp(codProxEstacao, "NULO") == 0) { //se o codProxEstacao for NULO, marcamos o campo como -1
        fwrite(&menosUm, sizeof(int),1,arqin);
    }
    else { //se nao for nulo, marcamos como o input lido
        int valorEmInt = atoi(codProxEstacao); //usamos isso para transformar o numero lido como string em int
        fwrite(&valorEmInt, sizeof(int), 1, arqin);
    }

    if (strcmp(distProxEstacao, "NULO") == 0) { //se o distProxEstacao for NULO, marcamos o campo como -1
        fwrite(&menosUm, sizeof(int),1,arqin);
    }
    else { //se nao for nulo, marcamos como o input lido
        int valorEmInt = atoi(distProxEstacao); //usamos isso para transformar o numero lido como string em int
        fwrite(&valorEmInt, sizeof(int), 1, arqin);
    }

    if (strcmp(codLinhaIntegra, "NULO") == 0) { //se o codLinhaIntegra for NULO, marcamos o campo como -1
        fwrite(&menosUm, sizeof(int),1,arqin);
    }
    else { //se nao for nulo, marcamos como o input lido
        int valorEmInt = atoi(codLinhaIntegra); //usamos isso para transformar o numero lido como string em int
        fwrite(&valorEmInt, sizeof(int), 1, arqin);
    }

    if (strcmp(codEstIntegra, "NULO") == 0) { //se o codEstIntegra for NULO, marcamos o campo como -1
        fwrite(&menosUm, sizeof(int),1,arqin);
    }
    else { //se nao for nulo, marcamos como o input lido
        int valorEmInt = atoi(codEstIntegra); //usamos isso para transformar o numero lido como string em int
        fwrite(&valorEmInt, sizeof(int), 1, arqin);
    }
    contBytes += 24; // 6 campos * 4 bytes = 24 bytes contados. Mesmo que os registros sejam nulos, seus tamanhos ainda sao de 4 bytes
    tamNomeEstacao = strlen(nomeEstacao); //vamos pegar o tamanho do nomeEstacao e nomeLinha
    tamNomeLinha = strlen(nomeLinha);
    fwrite(&tamNomeEstacao, sizeof(int), 1, arqin);
    contBytes += 4; //este campo é int

    if (strcmp(nomeEstacao, "NULO") == 0) { //se o nomeEstacao for NULO, marcamos o campo como 0
        fwrite(&zero, sizeof(int),1,arqin);
    }
    else { //se nao for nulo, marcamos como o input lido
        fwrite(nomeEstacao, sizeof(int), 1, arqin);
    }
    contBytes += tamNomeEstacao;  //o numero de bytes será o tamanho do nomeEstacao

    fwrite(&tamNomeLinha, sizeof(int), 1, arqin);
    contBytes += 4;     


    if (strcmp(nomeLinha, "NULO") == 0) { //se o nomeLinha for NULO, marcamos o campo como 0
        fwrite(&zero, sizeof(int),1,arqin);
    }
    else { //se nao for nulo, marcamos como o input lido
        fwrite(nomeLinha, sizeof(int), 1, arqin);
    }
    contBytes += tamNomeLinha;  //o numero de bytes será o tamanho do nomeLinha
     char lixo = '$'; //o espaço restante, preencheremos com $
     while (contBytes < 80) {
        fwrite(&lixo, sizeof(char), 1, arqin);
        contBytes++;
     }
}