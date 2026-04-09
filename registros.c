#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registros.h"
#include "fornecidas.h"

typedef struct registro_{
    //campos de tamanho fixo
    char removido; //indica se registro está logicamente removido ou não
    int proximo; //valor do RRN do proximo registro logicamente removido
    int codestacao;
    int codlinha;
    int codproxestacao;
    int distproxestacao;
    int codlinhaintegra;
    int codestintegra;
    //campos de tamanho variável e seus indicadores de tamanho
    int tamnomeestacao;
    char *nomeestacao;
    int tamnomelinha;
    char *nomelinha;
} REGISTRO;

//essa struct vai armazenar os critérios de busca que um registro deve atender
typedef struct criterios_{
    char *nomecampo;
    char *valorcampo;
}CRITERIOS;

//todos os parâmetros serão retirados do arquivo de entrada para se criar um registro
REGISTRO *createRecord(int codestacao, int codlinha, int codproxestacao, int distproxestacao, int codlinhaintegra, int codestintegra, char* nomeestacao, char *nomelinha)
{
    REGISTRO *registro = (REGISTRO *)malloc(sizeof(REGISTRO)); //memória alocada dinamicamente para o registro
    if (registro != NULL)
    {
        registro->removido = '0';
        registro->proximo = -1;
        registro->codestacao = codestacao;
        registro->codlinha = codlinha;
        registro->codproxestacao = codproxestacao;
        registro->distproxestacao = distproxestacao;
        registro->codlinhaintegra = codlinhaintegra;
        registro->codestintegra = codestintegra;
        registro->tamnomeestacao = strlen(nomeestacao);
        registro->nomeestacao = malloc(registro->tamnomeestacao + 1); //precisamos alocar a memória com strlen + 1 para que haja espaço suficiente para o \0
        strcpy(registro->nomeestacao, nomeestacao);
        registro->tamnomelinha = strlen(nomelinha);
        registro->nomelinha = malloc(registro->tamnomelinha + 1);
        strcpy(registro->nomelinha, nomelinha);
    }
    return registro;
}

// liberamos a memória alocada para um registro
void deleteRecord(REGISTRO *registro)
{
    if (registro->tamnomelinha != 0)
    {
        free(registro->nomelinha);
        (registro->nomelinha) = NULL;
    }
    free(registro->nomeestacao);
    (registro->nomeestacao) = NULL;
    free(registro);
    registro = NULL;
}

//essa struct armazena os critérios de busca que serão usados nas funções filterRecords, removeRecords e updateRecords
CRITERIOS *createCriteria(char *nomecampo, char *valorcampo)
{
    CRITERIOS *criterios = malloc(sizeof(CRITERIOS));
    if (criterios == NULL){
        return NULL;
    }  
    criterios->nomecampo = malloc(strlen(nomecampo)+1);
    strcpy(criterios->nomecampo, nomecampo);
    criterios->valorcampo = malloc(strlen(valorcampo)+1);
    strcpy(criterios->valorcampo, valorcampo);
    return criterios;
}

// libera a memória alocada para a struct de critérios de busca
void deleteCriteria(CRITERIOS *criterios)
{
    if(criterios != NULL)
    {
        free(criterios->nomecampo);
        criterios->nomecampo = NULL;
        free(criterios->valorcampo);
        criterios->valorcampo = NULL;
    }
    free(criterios);
    criterios = NULL;
}

//função que le todos os critérios de uma busca e devolve o array de structs
void readCriteria(int m, CRITERIOS **criterios)
{
    char nomecampo[30], valorcampo[30]; 
    for (int j = 0; j < m; j++)
    {
        scanf("%s", nomecampo); //vamos ler o nome do campo que vamos atualizar
        if (strcmp(nomecampo, "nomeEstacao") == 0 || strcmp(nomecampo, "nomeLinha") == 0){ //se esse campo for nomeEstacao ou nomeLinha, ele esta entre aspas
            ScanQuoteString(valorcampo);
        }
        else { //se nao for, nao está entre aspas e usamos scanf normal
            scanf("%s", valorcampo);
        }
        criterios[j] = createCriteria(nomecampo, valorcampo);
    }
}

//essa função pega uma linha do csv, separa em tokens e utiliza esses tokens pra criar um registro, direcionando os tokens para os campos do registro correspondentes
REGISTRO *recordFromCSV(char *buffer)
{
    int codestacao, codlinha, codproxest, distproxest, codlinhaintegra, codestintegra;  //todos os tokens que serão obtidos com o strsep
    char *nomeestacao, *nomelinha, *tokenTemp;                                          //e serão utilizados na criação de um registro
    buffer[strcspn(buffer, "\r\n")] = '\0'; //limpa possíveis \r e \n que possam ter sido lidos
    //tokenização de acordo com a ordem dos campos no arquivo csv
    codestacao = atoi(strsep(&buffer, ",")); //esse valor não pode ser nulo
    nomeestacao = strsep(&buffer, ",");
    tokenTemp = strsep(&buffer, ",");
    if (tokenTemp[0] != '\0') //caso o strsep se depare com 2 vírgulas consecutivas (oq significa um espaço nulo), ele retorna uma string "\0" 
    {
        codlinha = atoi(tokenTemp);
    }
    else codlinha = -1;                //no caso dos inteiros, se o campo for nulo, deve ser salvo como -1
    nomelinha = strsep(&buffer, ","); //se uma string for nula, a criação do registro ja considerará tamnomelinha como 0
    tokenTemp = strsep(&buffer, ",");
    if (tokenTemp[0] != '\0')
    {
        codproxest = atoi(tokenTemp);
    }
    else codproxest = -1; 
    tokenTemp = strsep(&buffer, ",");
    if (tokenTemp[0] != '\0')
    {
        distproxest = atoi(tokenTemp);
    }
    else distproxest = -1;
    tokenTemp = strsep(&buffer, ",");
    if (tokenTemp[0] != '\0')
    {
        codlinhaintegra = atoi(tokenTemp);
    }
    else codlinhaintegra = -1;
    tokenTemp = strsep(&buffer, ",");
    if (tokenTemp[0] != '\0')
    {
        codestintegra = atoi(tokenTemp);
    }
    else codestintegra = -1;
    //criação do registro usando todos os tokens coletados
    REGISTRO *registro = createRecord(codestacao, codlinha, codproxest, distproxest, codlinhaintegra, codestintegra, nomeestacao, nomelinha);
    return registro;
}

//le os bytes de um registro salvo num arquivo binário e transforma em um uma struct registro
REGISTRO *recordFromBin(FILE *arqbin, char removido)
{
    if (removido == '1') //registro está logicamente removido, não deve ser considerado
    {
        fseek(arqbin, 79, SEEK_CUR); //o ponteiro vai para o offset do próximo registro
        return NULL;
    }
    else
    {
        int codestacao, codlinha, codproxest, distproxest, codlinhaintegra, codestintegra, tamnomeestacao, tamnomelinha;  //todos os campos que serão lidos do registro
        
        fseek(arqbin, 4, SEEK_CUR); //ponteiro pula o campo registro->proximo
        //leitura de todos os campos do registro
        fread(&codestacao, sizeof(int), 1, arqbin);
        fread(&codlinha, sizeof(int), 1, arqbin);
        fread(&codproxest, sizeof(int), 1, arqbin);
        fread(&distproxest, sizeof(int), 1, arqbin);
        fread(&codlinhaintegra, sizeof(int), 1, arqbin);
        fread(&codestintegra, sizeof(int), 1, arqbin);

        fread(&tamnomeestacao, sizeof(int), 1, arqbin);
        char *nomeestacao = (char *)malloc(tamnomeestacao + 1); //alocação dinamica para a variavel local
        fread(nomeestacao, tamnomeestacao, 1, arqbin);
        fread(&tamnomelinha, sizeof(int), 1, arqbin);
        char *nomelinha = (char *)malloc(tamnomelinha + 1);
        fread(nomelinha, tamnomelinha, 1, arqbin);

        nomeestacao[tamnomeestacao] = '\0'; //adiciona o '\0' ao final das strings, já que elas estavam salvas sem ele no binário
        nomelinha[tamnomelinha] = '\0';
        
        int lixo = 80 - (tamnomelinha + tamnomeestacao + 37); //calculo da quantidade de lixo que deverá ser pulada para colocar o ponteiro do arquivo no offset do próximo registro
        fseek(arqbin, lixo, SEEK_CUR); //ponteiro direcionado para o offset do proximo registro
        
        REGISTRO *registro = createRecord(codestacao, codlinha, codproxest, distproxest, codlinhaintegra, codestintegra, nomeestacao, nomelinha);
        
        free(nomelinha); //liberando espaço alocado para as variáveis locais
        nomelinha = NULL;
        free(nomeestacao);
        nomeestacao = NULL;

        return registro; //registro criado com os dados lidos
    }
}

//le as informações de um registro inseridas por um usuário e cria o registro
REGISTRO *recordFromInput()
{
    int codestacao, codlinha, codproxest, distproxest, codlinhaintegra, codestintegra; //todos os campos que serão lidos do input
    char nomeestacao[30], nomelinha[30], stringtemp[30]; //stringtemp armazenará cada uma das strings lidas, isso é necessário quando o campo pode valer NULO
    scanf("%d", &codestacao);
    ScanQuoteString(nomeestacao);
    scanf("%s", stringtemp);
    if (strcmp(stringtemp, "NULO") == 0)
    {
        codlinha = -1;
    }
    else codlinha = atoi(stringtemp);
    ScanQuoteString(nomelinha);
    if (strcmp(nomelinha, "NULO") == 0)
    {
        nomelinha[0] = '\0';
    }
    scanf("%s", stringtemp);
    if (strcmp(stringtemp, "NULO") == 0)
    {
        codproxest = -1;
    }
    else codproxest = atoi(stringtemp);
    scanf("%s", stringtemp);
    if (strcmp(stringtemp, "NULO") == 0)
    {
        distproxest = -1;
    }
    else distproxest = atoi(stringtemp);
    scanf("%s", stringtemp);
    if (strcmp(stringtemp, "NULO") == 0)
    {
        codlinhaintegra = -1;
    }
    else codlinhaintegra = atoi(stringtemp);
    scanf("%s", stringtemp);
    if (strcmp(stringtemp, "NULO") == 0)
    {
        codestintegra = -1;
    }
    else codestintegra = atoi(stringtemp);
    REGISTRO *registro = createRecord(codestacao, codlinha, codproxest, distproxest, codlinhaintegra, codestintegra, nomeestacao, nomelinha);
    return registro;
}

//escreve os dados de um regitro em um arquivo binário
void writeRecordOnBin(REGISTRO *registro, FILE *fp)
{
    //escrevo cada um dos campos no arquivo
    fwrite(&registro->removido, sizeof(char), 1,fp); //1 byte
    fwrite(&registro->proximo, sizeof(int), 1,fp); //4 bytes
    fwrite(&registro->codestacao, sizeof(int), 1,fp); //4 bytes
    fwrite(&registro->codlinha, sizeof(int), 1,fp); //4 bytes
    fwrite(&registro->codproxestacao, sizeof(int), 1,fp); //4 bytes
    fwrite(&registro->distproxestacao, sizeof(int), 1,fp); //4 bytes
    fwrite(&registro->codlinhaintegra, sizeof(int), 1,fp); //4 bytes
    fwrite(&registro->codestintegra, sizeof(int), 1,fp); //4 bytes
    fwrite(&registro->tamnomeestacao, sizeof(int), 1,fp); //4 bytes
    if (registro->tamnomeestacao != 0)
    {
        fwrite(registro->nomeestacao, registro->tamnomeestacao, 1,fp); //passando o espaço desejado dessa forma, eu excluo o \0 (que não deve ser salvo)
    }
    fwrite(&registro->tamnomelinha, sizeof(int), 1,fp); //4 bytes
    if (registro->tamnomelinha != 0)
    {
        fwrite(registro->nomelinha, registro->tamnomelinha, 1,fp);
    }
    //temos no total (37 + tamnomelinha + tamnomeestacao) bytes, devemos completar o que falta pra 80 bytes com lixo ("$")
    int qtdlixo = 80 - (37 + registro->tamnomelinha + registro->tamnomeestacao); 
    char lixo = '$';
    for (int i = 0; i < qtdlixo; i++)
    {
        fwrite(&lixo, sizeof(char),1, fp); //escreve todo o lixo necessário até o final do registro
    }
    return;
}

//essa função vai pegar um registro e imprimir seus campos conforme a formatação pedida pelas especificações do trabalho
void printRecord(REGISTRO *registro)
{
    if (registro == NULL) //se o registro for NULO, nada deve ser impresso
    {
        return;
    }
    printf("%d %s ", registro->codestacao, registro->nomeestacao);
    if (registro->codlinha == -1) //para os campos que não existem, deve ser printado como "NULO"
    {
        printf("NULO ");
    }
    else printf("%d ", registro->codlinha);
    if (registro->tamnomelinha == 0)// para o campo "nomelinha" ser nulo, tamnomelinha deve valer 0
    {
        printf("NULO ");
    }
    else printf("%s ", registro->nomelinha);
    if (registro->codproxestacao == -1)
    {
        printf("NULO ");
    }
    else printf("%d ", registro->codproxestacao);
    if (registro->distproxestacao == -1)
    {
        printf("NULO ");
    }
    else printf("%d ", registro->distproxestacao);
    if (registro->codlinhaintegra == -1)
    {
        printf("NULO ");
    }
    else printf("%d ", registro->codlinhaintegra);
    if (registro->codestintegra == -1)
    {
        printf("NULO\n");
    }
    else printf("%d\n", registro->codestintegra);
}

//função para verificar se um registro cumpre os critérios de uma busca
int recordMeetsCriteria(REGISTRO *registro, int m, CRITERIOS **criterios)
{
    for (int i = 0; i < m; i++) //m é a quantidade de critérios impostos na busca
    {
        if (strcmp(criterios[i]->nomecampo, "codEstacao") == 0)
        {
            if(registro->codestacao != atoi(criterios[i]->valorcampo)) //"valorcampo" é sempre lido como string. se o campo em questão for um inteiro, deve ser feita a conversão usando o atoi
            {
                return 0;
            }
        }
        if (strcmp(criterios[i]->nomecampo, "codLinha") == 0)
        {
            if(strcmp(criterios[i]->valorcampo, "NULO") == 0)
            {
                if(registro->codlinha != -1)
                {
                    return 0;
                }
            }
            else
            {
                if(registro->codlinha != atoi(criterios[i]->valorcampo))
                {
                    return 0;
                }
            }
        }
        if (strcmp(criterios[i]->nomecampo, "codProxEstacao") == 0)
        {
            if(strcmp(criterios[i]->valorcampo, "NULO") == 0)
            {
                if(registro->codproxestacao != -1)
                {
                    return 0;
                }
            }
            else
            {
                if(registro->codproxestacao != atoi(criterios[i]->valorcampo))
                {
                    return 0;
                }
            }
        }
        if (strcmp(criterios[i]->nomecampo, "distProxEstacao") == 0)
        {
            if(strcmp(criterios[i]->valorcampo, "NULO") == 0)
            {
                if(registro->distproxestacao != -1)
                {
                    return 0;
                }
            }
            else
            {
                if(registro->distproxestacao != atoi(criterios[i]->valorcampo))
                {
                    return 0;
                }
            }
        }
        if (strcmp(criterios[i]->nomecampo, "codLinhaIntegra") == 0)
        {
            if(strcmp(criterios[i]->valorcampo, "NULO") == 0)
            {
                if(registro->codlinhaintegra != -1)
                {
                    return 0;
                }
            }
            else
            {
                if(registro->codlinhaintegra != atoi(criterios[i]->valorcampo))
                {
                    return 0;
                }
            }
        }
        if (strcmp(criterios[i]->nomecampo, "codEstIntegra") == 0)
        {
            if(strcmp(criterios[i]->valorcampo, "NULO") == 0)
            {
                if(registro->codestintegra != -1)
                {
                    return 0;
                }
            }
            else
            {
                if(registro->codestintegra != atoi(criterios[i]->valorcampo))
                {
                    return 0;
                }
            }
        }
        if (strcmp(criterios[i]->nomecampo, "nomeEstacao") == 0)
        {
            if(strcmp(registro->nomeestacao,criterios[i]->valorcampo) != 0)
            {
                return 0;
            }
        }
        if (strcmp(criterios[i]->nomecampo, "nomeLinha") == 0)
        {
            if(strcmp(criterios[i]->valorcampo, "NULO") == 0)
            {
                if(registro->tamnomelinha != 0)
                {
                    return 0;
                }
            }
            else
            {
                if(strcmp(registro->nomelinha, criterios[i]->valorcampo) != 0)
                {
                    return 0;
                }
            }
        }
    }
    return 1; //o registro atendeu a todos os critérios
}

//função para se acessar o campo nomeestacao de um registro
char *getNomeEstacao(REGISTRO *registro)
{
    if(registro != NULL)
    {
        return (registro->nomeestacao);
    }
}

//função para se acessar o campo codestacao de um registro
int getCodEstacao(REGISTRO *registro)
{
    if(registro != NULL)
    {
        return (registro->codestacao);
    }
}

//função para se acessar o campo codproxestacao de um registro
int getCodProxEstacao(REGISTRO *registro)
{
    if(registro != NULL)
    {
        return (registro->codproxestacao);
    }
}

//função para se alterar o valor do campo "nomecampo" de uma struct criterios
void setNomeCampo(CRITERIOS *criterios, char *nomecampo)
{
    strcpy(criterios->nomecampo, nomecampo);
}

//função para se alterar o valor do campo "valorcampo" de uma struct criterios
void setValorCampo(CRITERIOS *criterios, char *valorcampo)
{
    strcpy(criterios->valorcampo, valorcampo);
}

//função para se alterar o valor do campo "proximo" de um registro
void setProximo(REGISTRO *registro, int valor)
{
    registro->proximo = valor;
}

//função para marcar um registro como logicamente removido
void removeRecord(REGISTRO *registro) {
    registro->removido = '1';
}

void atualizarCamposRegistro(REGISTRO *registro, int atts, CRITERIOS **criteriosAtt) { //funcao que pega as atualizacoes (criteriosAtt) e escreve no registro
    for (int i = 0; i < atts; i++) { //esse loop deve se repetir para cada atualizacao de campo que vamos fazer
        char *campo = criteriosAtt[i]->nomecampo; //guardaremos aqui qual o nome do campo
        char *valor = criteriosAtt[i]->valorcampo; //e aqui o valor armazenado nesse campo

        if (strcmp(campo, "nomeEstacao") == 0) { //devemos tratar do caso especial em que o campo em questao seja o nomeestacao, pois é uma string de tamanho variavel, nao basta apenas substituir o que ha li
            if (registro->nomeestacao != NULL) { //como essa string de tamanho variavel usa alocacao dinamica, liberamos esse espaço ocupado
                free(registro->nomeestacao);
            }
            
            if (strcmp(valor, "NULO") == 0) { //se tivermos que ocupar esse campo com NULO, nao deve haver nada ali e seu tamanho sera 0
                registro->tamnomeestacao = 0;
                registro->nomeestacao = NULL;
            } else { //para qualquer outro caso, apenas atualizamos o valor do campo e seu tamanho
                registro->tamnomeestacao = strlen(valor);
                registro->nomeestacao = malloc((registro->tamnomeestacao + 1) * sizeof(char));
                strcpy(registro->nomeestacao, valor);
            }
        } 
        else if (strcmp(campo, "nomeLinha") == 0) { //a mesma logica do nomeestacao se aplica ao nomelinha
            if (registro->nomelinha != NULL) {
                free(registro->nomelinha);
            }
            
            if (strcmp(valor, "NULO") == 0) {
                registro->tamnomelinha = 0;
                registro->nomelinha = NULL;
            } else {
                registro->tamnomelinha = strlen(valor);
                registro->nomelinha = malloc((registro->tamnomelinha + 1) * sizeof(char));
                strcpy(registro->nomelinha, valor);
            }
        }
        else if (strcmp(campo, "codEstacao") == 0) { //ja para o caso de quaisquer outros campos, usamos apenas a logica de substituicao
            if (strcmp(valor, "NULO") == 0) {//nesses registros, se o valor a ser inserido (na atualizacao) for NULO, inserimos -1
                registro->codestacao = -1;
            }
            else {          //se nao, inserimos o valor em int (atoi)
                registro->codestacao = atoi(valor);
            }
        
        } 
        else if (strcmp(campo, "codLinha") == 0) {
             if (strcmp(valor, "NULO") == 0) {
                registro->codlinha = -1;
            }
            else {         
                registro->codlinha = atoi(valor);
            }
        } 
        else if (strcmp(campo, "codProxEstacao") == 0) {
             if (strcmp(valor, "NULO") == 0) {
                registro->codproxestacao = -1;
            }
            else {   
                registro->codproxestacao = atoi(valor);
            }
        } 
        else if (strcmp(campo, "distProxEstacao") == 0) {
            if (strcmp(valor, "NULO") == 0) {
                registro->distproxestacao = -1;
            }
            else {          
                registro->distproxestacao = atoi(valor);
            }
        } 
        else if (strcmp(campo, "codLinhaIntegra") == 0) {
            if (strcmp(valor, "NULO") == 0) {
                registro->codlinhaintegra = -1;
            }
            else {        
                registro->codlinhaintegra = atoi(valor);
            }
        } 
        else if (strcmp(campo, "codEstIntegra") == 0) {
            if (strcmp(valor, "NULO") == 0) {
                registro->codestintegra = -1;
            }
            else {          
                registro->codestintegra = atoi(valor);
            }
        }
    }
}

void contarEstacoesEPares(FILE *arqin, int *nroestacoes, int *nroparesestacao)
{
    char *listanomes[10000];
    int contanomes = 0; //esta variável vai guardar quantos nomes unicos ja encontramos
    int listaestacao[10000], listaprox[10000]; //vamos guardar aqui os codestacao e os codproxestacao
    int contapares = 0;
    fseek(arqin, 17, SEEK_SET); //vamos começar a leitura após o cabeçalho 
    
    char removido;
    while (fread(&removido, sizeof(char), 1, arqin))  //só paramos se chegarmos ao fim do arquivo
    {  
        REGISTRO *registro = recordFromBin(arqin, removido); //vamos guardar o registro em "registro"
        if (registro != NULL)  //como o recordFromBin devolve NULL para registros com status removidos, devemos ver se o registro não foi removido
        {
            int existenome = 0;    //começamos assumindo que ainda nao vimos o nome
            for (int i = 0; i < contanomes; i++) 
            {
                if (strcmp(registro->nomeestacao, listanomes[i]) == 0) 
                { 
                    existenome = 1; //se o nome da estacao lido ja estiver na lista de nomes, entao o nome ja existia e mudamos o existenome
                    break;
                }
            }
            if (existenome == 0)  //se o existenome continua sendo 0, entao temos aqui um nome novo, que deve ser anotado na listanomes
            {
                listanomes[contanomes] = (char *)malloc((strlen(registro->nomeestacao) + 1) * sizeof(char)); //alocamos espaço para escrever na listanomes
                strcpy(listanomes[contanomes], registro->nomeestacao);
                contanomes++; 
            }
            
            int existepar = 0; //agora devemos contar os pares, seguindo a mesma logica
            if (registro->codestacao != -1 && registro->codproxestacao != -1) 
            {
                for (int i = 0; i < contapares; i++)
                {
                    if (((registro->codestacao == listaestacao[i] && registro->codproxestacao == listaprox[i]) || (registro->codestacao == listaprox[i] && registro->codproxestacao == listaestacao[i])) && registro->codproxestacao != -1) //se tivermos um par
                    { 
                        existepar = 1;
                        break;
                    }
                }
                if (existepar == 0) //se o par nao existe, basta adicionar
                { 
                    listaestacao[contapares] = registro->codestacao;
                    listaprox[contapares] = registro->codproxestacao;
                    contapares++;
                }
            }
            deleteRecord(registro); 
        }
    }
    *nroestacoes = contanomes; //o nroestacoes vai guardar o numero de nomes unicos
    *nroparesestacao = contapares; //o mesmo para o nroparesestacao
    for (int i = 0; i < contanomes; i++)  //liberamos o espaço na listanomes
    {
            free(listanomes[i]);
    }
}

