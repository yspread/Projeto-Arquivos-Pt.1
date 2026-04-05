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
} REGISTRO;

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
        registro->nomeEstacao = malloc(registro->tamNomeEstacao + 1);
        strcpy(registro->nomeEstacao, nomeEstacao);
        registro->tamNomeLinha = strlen(nomeLinha);
        if (registro->tamNomeLinha != 0) //em caso do campo nomeLinha ser nulo, não devemos alocar memória
        {
            registro->nomeLinha = malloc(registro->tamNomeLinha + 1);
        }
        strcpy(registro->nomeLinha, nomeLinha);
    }
    return registro;
}

void deleteRecord(REGISTRO *registro)
{
    if (registro->tamNomeLinha != 0)
    {
        free(registro->nomeLinha);
        (registro->nomeLinha) = NULL;
    }
    free(registro->nomeEstacao);
    (registro->nomeEstacao) = NULL;
    free(registro);
    registro = NULL;
}

CRITERIOS *createCriteria(char *nomeCampo, char *valorCampo)
{
    CRITERIOS *criterios = malloc(sizeof(CRITERIOS));
    if (criterios != NULL)
    {
        criterios->nomeCampo = malloc(strlen(nomeCampo)+1);
        strcpy(criterios->nomeCampo, nomeCampo);
        criterios->valorCampo = malloc(strlen(valorCampo)+1);
        strcpy(criterios->valorCampo, valorCampo);
        return criterios;
    }
    else
    {
        return NULL;
    }
}

void deleteCriteria(CRITERIOS *criterios)
{
    if(criterios != NULL)
    {
        free(criterios->nomeCampo);
        criterios->nomeCampo = NULL;
        free(criterios->valorCampo);
        criterios->valorCampo = NULL;
    }
    free(criterios);
    criterios = NULL;
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
        fseek(arqbin, 4, SEEK_CUR); //ponteiro pula o campo registro->proximo
        //leitura de todos os campos do registro
        fread(&codEstacao, sizeof(int), 1, arqbin);
        fread(&codLinha, sizeof(int), 1, arqbin);
        fread(&codProxEst, sizeof(int), 1, arqbin);
        fread(&distProxEst, sizeof(int), 1, arqbin);
        fread(&codLinhaIntegra, sizeof(int), 1, arqbin);
        fread(&codEstIntegra, sizeof(int), 1, arqbin);
        fread(&tamNomeEstacao, sizeof(int), 1, arqbin);
        char *nomeEstacao = malloc(tamNomeEstacao + 1); //é necessário alocar com +1 para poder colocar o '\0' que não foi salvo
        fread(nomeEstacao, tamNomeEstacao, 1, arqbin);
        fread(&tamNomeLinha, sizeof(int), 1, arqbin);
        char *nomeLinha = malloc(tamNomeLinha + 1);
        fread(nomeLinha, tamNomeLinha, 1, arqbin);
        nomeEstacao[tamNomeEstacao] = '\0';
        nomeLinha[tamNomeLinha] = '\0';
        int lixo = 80 - (tamNomeLinha + tamNomeEstacao + 37); //a quantidade de lixo que deverá ser pulada para colocar o ponteiro do arquivo no offset do próximo registro
        fseek(arqbin, lixo, SEEK_CUR); //ponteiro direcionado para o offset do proximo registro
        REGISTRO *registro = createRecord(codEstacao, codLinha, codProxEst, distProxEst, codLinhaIntegra, codEstIntegra, nomeEstacao, nomeLinha);
        free(nomeEstacao);
        free(nomeLinha);
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
        if (strcmp(criterios[i]->nomeCampo, "codEstacao") == 0)
        {
            if(registro->codEstacao != atoi(criterios[i]->valorCampo))
            {
                return 0;
            }
        }
        if (strcmp(criterios[i]->nomeCampo, "codLinha") == 0)
        {
            if(strcmp(criterios[i]->valorCampo, "NULO") == 0)
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
        if (strcmp(criterios[i]->nomeCampo, "codProxEstacao") == 0)
        {
            if(strcmp(criterios[i]->valorCampo, "NULO") == 0)
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
        if (strcmp(criterios[i]->nomeCampo, "distProxEstacao") == 0)
        {
            if(strcmp(criterios[i]->valorCampo, "NULO") == 0)
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
        if (strcmp(criterios[i]->nomeCampo, "codLinhaIntegra") == 0)
        {
            if(strcmp(criterios[i]->valorCampo, "NULO") == 0)
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
        if (strcmp(criterios[i]->nomeCampo, "codEstIntegra") == 0)
        {
            if(strcmp(criterios[i]->valorCampo, "NULO") == 0)
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
        if (strcmp(criterios[i]->nomeCampo, "nomeEstacao") == 0)
        {
            if(strcmp(registro->nomeEstacao,criterios[i]->valorCampo) != 0)
            {
                return 0;
            }
        }
        if (strcmp(criterios[i]->nomeCampo, "nomeLinha") == 0)
        {
            if(strcmp(criterios[i]->valorCampo, "NULO") == 0)
            {
                if(registro->tamNomeLinha != 0)
                {
                    return 0;
                }
            }
            else
            {
                if(strcmp(registro->nomeLinha, criterios[i]->valorCampo) != 0)
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

void setNomeCampo(CRITERIOS *criterios, char *nomeCampo)
{
    strcpy(criterios->nomeCampo, nomeCampo);
}

void setValorCampo(CRITERIOS *criterios, char *valorCampo)
{
    strcpy(criterios->valorCampo, valorCampo);
}

void setProximo(REGISTRO *registro, int valor) {
    registro->proximo = valor;
}

void setRemovido(REGISTRO *registro) {
    registro->removido = '1';
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

    if (strcmp(distProxEstacao, "NULO") == 0) 
    { //se o distProxEstacao for NULO, marcamos o campo como -1
        fwrite(&menosUm, sizeof(int),1,arqin);
    }
    else 
    { //se nao for nulo, marcamos como o input lido
        int valorEmInt = atoi(distProxEstacao); //usamos isso para transformar o numero lido como string em int
        fwrite(&valorEmInt, sizeof(int), 1, arqin);
    }

    if (strcmp(codLinhaIntegra, "NULO") == 0) 
    { //se o codLinhaIntegra for NULO, marcamos o campo como -1
        fwrite(&menosUm, sizeof(int),1,arqin);
    }
    else 
    { //se nao for nulo, marcamos como o input lido
        int valorEmInt = atoi(codLinhaIntegra); //usamos isso para transformar o numero lido como string em int
        fwrite(&valorEmInt, sizeof(int), 1, arqin);
    }

    if (strcmp(codEstIntegra, "NULO") == 0) 
    { //se o codEstIntegra for NULO, marcamos o campo como -1
        fwrite(&menosUm, sizeof(int),1,arqin);
    }
    else 
    { //se nao for nulo, marcamos como o input lido
        int valorEmInt = atoi(codEstIntegra); //usamos isso para transformar o numero lido como string em int
        fwrite(&valorEmInt, sizeof(int), 1, arqin);
    }
    contBytes += 24; // 6 campos * 4 bytes = 24 bytes contados. Mesmo que os registros sejam nulos, seus tamanhos ainda sao de 4 bytes

    if (strcmp(nomeEstacao, "NULO") == 0 || strlen(nomeEstacao) == 0) {
        tamNomeEstacao = 0; // se for nulo, o tamanho é 0
    } else {
        tamNomeEstacao = strlen(nomeEstacao); // se nao for, guardamos na variavel
    }

    fwrite(&tamNomeEstacao, sizeof(int), 1, arqin); //escrevemos o tamanho da string
    contBytes += 4;

    if (tamNomeEstacao > 0) { //se o tamanho for maior que 0 anotamos o nome da estacao
        fwrite(nomeEstacao, sizeof(char), tamNomeEstacao, arqin);
        contBytes += tamNomeEstacao;
    }
    //faremos o mesmo com o nomeLinha
    if (strcmp(nomeLinha, "NULO") == 0 || strlen(nomeLinha) == 0) {
        tamNomeLinha = 0; // se for nulo, o tamanho é 0
    } else {
        tamNomeLinha = strlen(nomeLinha); // se nao for, guardamos na variavel
    }

    fwrite(&tamNomeLinha, sizeof(int), 1, arqin); //escrevemos o tamanho da string
    contBytes += 4;

    if (tamNomeLinha > 0) { //se o tamanho for maior que 0 anotamos o nome da estacao
        fwrite(nomeLinha, sizeof(char), tamNomeLinha, arqin);
        contBytes += tamNomeLinha;
    }
    char lixo = '$'; //o espaço restante, preencheremos com $
    while (contBytes < 80)
    {
        fwrite(&lixo, sizeof(char), 1, arqin);
        contBytes++;
    }
}

void atualizarCamposRegistro(REGISTRO *registro, int atts, CRITERIOS **criteriosAtt) { //funcao que pega as atualizacoes (criteriosAtt) e escreve no registro
    for (int i = 0; i < atts; i++) { //esse loop deve se repetir para cada atualizacao de campo que vamos fazer
        char *campo = criteriosAtt[i]->nomeCampo; //guardaremos aqui qual o nome do campo
        char *valor = criteriosAtt[i]->valorCampo; //e aqui o valor armazenado nesse campo

        if (strcmp(campo, "nomeEstacao") == 0) { //devemos tratar do caso especial em que o campo em questao seja o nomeEstacao, pois é uma string de tamanho variavel, nao basta apenas substituir o que ha li
            if (registro->nomeEstacao != NULL) { //como essa string de tamanho variavel usa alocacao dinamica, liberamos esse espaço ocupado
                free(registro->nomeEstacao);
            }
            
            if (strcmp(valor, "NULO") == 0) { //se tivermos que ocupar esse campo com NULO, nao deve haver nada ali e seu tamanho sera 0
                registro->tamNomeEstacao = 0;
                registro->nomeEstacao = NULL;
            } else { //para qualquer outro caso, apenas atualizamos o valor do campo e seu tamanho
                registro->tamNomeEstacao = strlen(valor);
                registro->nomeEstacao = malloc((registro->tamNomeEstacao + 1) * sizeof(char));
                strcpy(registro->nomeEstacao, valor);
            }
        } 
        else if (strcmp(campo, "nomeLinha") == 0) { //a mesma logica do nomeEstacao se aplica ao nomeLinha
            if (registro->nomeLinha != NULL) {
                free(registro->nomeLinha);
            }
            
            if (strcmp(valor, "NULO") == 0) {
                registro->tamNomeLinha = 0;
                registro->nomeLinha = NULL;
            } else {
                registro->tamNomeLinha = strlen(valor);
                registro->nomeLinha = malloc((registro->tamNomeLinha + 1) * sizeof(char));
                strcpy(registro->nomeLinha, valor);
            }
        }
        else if (strcmp(campo, "codEstacao") == 0) { //ja para o caso de quaisquer outros campos, usamos apenas a logica de substituicao
            if (strcmp(valor, "NULO") == 0) {//nesses registros, se o valor a ser inserido (na atualizacao) for NULO, inserimos -1
                registro->codEstacao = -1;
            }
            else {          //se nao, inserimos o valor em int (atoi)
                registro->codEstacao = atoi(valor);
            }
        
        } 
        else if (strcmp(campo, "codLinha") == 0) {
             if (strcmp(valor, "NULO") == 0) {
                registro->codLinha = -1;
            }
            else {         
                registro->codLinha = atoi(valor);
            }
        } 
        else if (strcmp(campo, "codProxEstacao") == 0) {
             if (strcmp(valor, "NULO") == 0) {
                registro->codProxEstacao = -1;
            }
            else {   
                registro->codProxEstacao = atoi(valor);
            }
        } 
        else if (strcmp(campo, "distProxEstacao") == 0) {
            if (strcmp(valor, "NULO") == 0) {
                registro->distProxEstacao = -1;
            }
            else {          
                registro->distProxEstacao = atoi(valor);
            }
        } 
        else if (strcmp(campo, "codLinhaIntegra") == 0) {
            if (strcmp(valor, "NULO") == 0) {
                registro->codLinhaIntegra = -1;
            }
            else {        
                registro->codLinhaIntegra = atoi(valor);
            }
        } 
        else if (strcmp(campo, "codEstIntegra") == 0) {
            if (strcmp(valor, "NULO") == 0) {
                registro->codEstIntegra = -1;
            }
            else {          
                registro->codEstIntegra = atoi(valor);
            }
        }
    }
}

void contarEstacoesEPares(FILE *arqin, int *nroEstacoes, int *nroParesEstacao) {
    char *listaNomes[10000];
    int contaNomes = 0; //esta variável vai guardar quantos nomes unicos ja encontramos
    int listaEstacao[10000], listaProx[10000]; //vamos guardar aqui os codEstacao e os codProxEstacao
    int contaPares = 0;

    fseek(arqin, 17, SEEK_SET); //vamos começar a leitura após o cabeçalho 
    
    char temp;
    while (fread(&temp, sizeof(char), 1, arqin))  //só paramos se chegarmos ao fim do arquivo
    { 
        fseek(arqin, -1, SEEK_CUR); 
        REGISTRO *registro = recordFromBin(arqin); //vamos guardar o registro em "registro"
        if (registro != NULL)  //como o recordFromBin devolve NULL para registros com status removidos, devemos ver se o registro não foi removido
        {
            int existeNome = 0;    //começamos assumindo que ainda nao vimos o nome
            for (int i = 0; i < contaNomes; i++) 
            {
                if (strcmp(registro->nomeEstacao, listaNomes[i]) == 0) 
                { 
                    existeNome = 1; //se o nome da estacao lido ja estiver na lista de nomes, entao o nome ja existia e mudamos o existeNome
                    break;
                }
            }
            if (existeNome == 0)  //se o existeNome continua sendo 0, entao temos aqui um nome novo, que deve ser anotado na listaNomes
            {
                listaNomes[contaNomes] = (char *)malloc((strlen(registro->nomeEstacao) + 1) * sizeof(char)); //alocamos espaço para escrever na listaNomes
                strcpy(listaNomes[contaNomes], registro->nomeEstacao);
                contaNomes++; 
            }
            
            int existePar = 0; //agora devemos contar os pares, seguindo a mesma logica
            if (registro->codEstacao != -1 && registro->codProxEstacao != -1) 
            {
                for (int i = 0; i < contaPares; i++)
                {
                    if (((registro->codEstacao == listaEstacao[i] && registro->codProxEstacao == listaProx[i]) || (registro->codEstacao == listaProx[i] && registro->codProxEstacao == listaEstacao[i])) && registro->codProxEstacao != -1) //se tivermos um par
                    { 
                        existePar = 1;
                        break;
                    }
                }
                if (existePar == 0) //se o par nao existe, basta adicionar
                { 
                    listaEstacao[contaPares] = registro->codEstacao;
                    listaProx[contaPares] = registro->codProxEstacao;
                    contaPares++;
                }
            }
            deleteRecord(registro); 
        }
    }
    *nroEstacoes = contaNomes; //o nroEstacoes vai guardar o numero de nomes unicos
    *nroParesEstacao = contaPares; //o mesmo para o nroParesEstacao
    for (int i = 0; i < contaNomes; i++)  //liberamos o espaço na listaNomes
    {
            free(listaNomes[i]);
    }
}

