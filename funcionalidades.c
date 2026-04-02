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
void readRecords(char *arqentrada, char *arqsaida){
    char **listanomesestacoes; //vai armazenar os nomes de estacoes que ja foram registrados, a fim de não contar os repetidos
    int contanomes = 0; //conta quantos nomes diferentes tem na lista
    int nomeestacaotemp; //armazenará temporariamente o nome da estacao de um registro
    int *listacodestacoes, *listacodproxestacoes; //lista vai armazenar os pares de codEstacao e codProxEstacao diferentes
    int contapares = 0; //conta quantos pares codEstacao, codProxEstaco diferentes tem na lista
    int codestacaotemp, codproxestacaotemp;; //armazenarao temporariamente o codigo da estacao e o codigo da proxima estacao de um registro
    HEADER *header = createHeader(); //crio uma header que será preenchida conforme os registros são lidos
    REGISTRO *registrotemp; //esse registro temporário armazenará o registro que vai ser escrito no binário
    char buffer[100]; //armazenará a linha que foi lida do csv
    FILE *arqin = fopen(arqentrada, "r"); //abertura do arquivo de entrada para leitura
    if (arqin == NULL)
    {
        printf("Não foi possível abrir o arquivo \"%s\"", arqentrada);
        return;
    }
    FILE *arqout = fopen(arqsaida, "wb"); //abertura do arquivo de saida para escrita em binário
    changeHeaderStatus(header); //o arquivo foi aberto para escrita, status atualizado
    fwrite(header, 17, 1, arqout); //escrita dos 17 bytes do cabeçalho inicial que será posteriormente sobrescrito(posso escrever a struct inteira de uma vez, visto que todos os campos tem tamanho fixo)
    fgets(buffer, 100, arqin); //primeira linha deve ser ignorada
    while (fgets(buffer, 100, arqin) != NULL) //enquanto não acaba o arquivo
    {
        registrotemp = recordFromCSV(buffer); //crio registro temporário com os valores presentes na linha
        writeRecordOnBin(registrotemp, arqout); //escrevo os 80 bytes do registro no arquivo binário
        setProxRRN(header, (getProxRRN(header)+1));
        /*descubro se o nome da estacao do registro atual é novo, se for, atualizo header->nroEstacoes, contando + 1*/
        nomeestacaotemp = getNomeEstacao(registrotemp);
        if (contanomes == 0) 
        {
            listanomesestacoes[contanomes] = nomeestacaotemp;
            contanomes++;
        }
        for(int i = 0; i < contanomes; i++)
        {
            if(listanomesestacoes[i] == nomeestacaotemp) //se eu achar algum nome igual na lista, nao conto como nome novo de estacao
            {
                break;
            }
            if(i == (contanomes - 1)) //se chegou no final da lista e não achou nenhum nome igual, eu conto++ e adiciono o novo nome na lista
            {
                listanomesestacoes[contanomes] = nomeestacaotemp;
                contanomes++;
            }
        }
        /*faço a mesma coisa que com o numero de com o nome da estacao, so que agora com o par (codEstacao, codProxEstacao)
        se for um par novo, conto + 1 em header->nroParesEstacao*/
        codestacaotemp = getCodEstacao(registrotemp);
        codproxestacaotemp = getCodProxEstacao(registrotemp);
        if (contapares == 0)
        {
            listacodestacoes[contapares] = codestacaotemp;
            listacodproxestacoes[contapares] = codproxestacaotemp;
            contapares++;
        }
        for(int i = 0; i < contapares; i++)
        {
            if(listacodestacoes[i] == codestacaotemp && listacodproxestacoes[i] == codproxestacaotemp) //se eu achar um par igual na lista, nao conto como par novo
            {
                break;
            }
            if (i == (contapares - 1)) //se chegou no final da lista e não achou nenhum par igual, conto++ e adiciono o par nas respectivas listas
            {
                listacodestacoes[contapares] = codestacaotemp;
                listacodproxestacoes[contapares] = codproxestacaotemp;
                contapares++;
            }
        }
        if (registrotemp != NULL)
        {
            free(registrotemp);
            registrotemp = NULL;
        }
    }
    fseek(arqout, 0, SEEK_SET); //coloco o ponteiro no inicio do arquivo
    changeHeaderStatus(header); //o arquivo será fechado, devo indicar isso com status = 1
    setNroEstacoes(header, contanomes); //atualizo o valor de header->nroEstacoes
    setNroParesEstacao(header, contapares); //atualizo o valor de header->nroParesEstacao
    fwrite(header, 17, 1, arqout); //sobrescrevo a header antiga com os novos dados da header
    fclose(arqin);
    fclose(arqout);
    BinarioNaTela(arqout);
    return;
}

/*essa função vai imprimir os campos de cada registro do arquivo binário de entrada
"arqentrada"
Vamos ler um registro por vez e printá-lo*/
void showRecords(char *arqentrada)
{
    char temp; //variável temporária, só serve para ser usada no fread do loop,
    REGISTRO *registrotemp;
    FILE *arqin = fopen(arqentrada, "rb"); //abertura do arquivo binário a ser lido
    if (arqin == NULL)
    {
        printf("Não foi possível abrir o arquivo \"%s\"", arqentrada);
        return;
    }
    fseek(arqin, 17, SEEK_SET); //ponteiro pula o cabeçalho, não será útil para a impressão dos registros
    while (fread(temp, 1, 1, arqin)) //fread serve para verificar se ainda há registros a serem lidos
    {
        fseek(arqin, -1, SEEK_CUR); //volto um byte com o ponteiro do arquivo para que os registros sejam lidos adequadamente com a função RecordFromBin
        registrotemp = recordFromBin(arqin); //crio registro temporário com os dados retirados do arquivo binário
        printRecord(registrotemp); //imprimo o registro se ele não tiver sido logicamente removido
        if (registrotemp != NULL)
        {
            free(registrotemp);
            registrotemp = NULL;
        }
    }
    fclose(arqin);
    return;
}

/*essa função vai receber como parametro um arquivo binário e vai fazer n buscas
por campo e valor do campo no arquivo. vai imprimir os resultados compatíveis
n equivale ao número de buscas diferentes que serão feitas*/
void filterRecords(char *arqentrada, int n)
{
    FILE *arqin = fopen(arqentrada, "rb"); //abertura do arquivo binário no qual será feita a busca
    if (arqin == NULL)
    {
        printf("Não foi possível abrir o arquivo \"%s\"", arqentrada);
        return;
    }
    int m;
    REGISTRO *registrotemp;
    char *stringtemp;
    char temp;
    fseek(arqin, 17, SEEK_SET);
    char *nomeCampos, *valorCampos;
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &m);
        CRITERIOS *criterios[m];
        for (int j = 0; j < m; j++)
        {
            scanf("%s", stringtemp);
            setNomeCampo(criterios[j], stringtemp);
            ScanQuoteString(stringtemp);
            setValorCampo(criterios[j], stringtemp);
        }
        while(fread(&temp, sizeof(char), 1, arqin))
        {   
            fseek(arqin, -1, SEEK_CUR);
            registrotemp = recordFromBin(arqin);
            if (recordMeetsCriteria(registrotemp, m, criterios))
            {
                printRecord(registrotemp);
            }
        }
    }
}


/*essa funcao vai remover um registro de acordo com uma especificação (criterio) do usuario, usando a funcionalidade 3
os registros removidos vao compor uma pilha para guardar os endereços e colocar novos registros
nesses endereços */
void removeRecord(char *arqentrada, int n)  {
    FILE *arqin = fopen(arqentrada, "rb+"); //vamos abrir o arquivo em wb para ler e escrever
    if (arqin == NULL)
    {
        printf("Não foi possível abrir o arquivo \"%s\"", arqentrada);
        return;
    }
    HEADER *headertemp = createHeader(); //vamos usar uma header temporario para guardar as infos e adiciona-las ao header principal depois
    changeHeaderStatus(headertemp); 
    fseek(arqin, 1, SEEK_SET); //vamos para o campo topo do cabeçalho
    int atualTopo; 
    fread(&atualTopo, sizeof(int), 1, arqin); //vamos armazenar o atual topo do cabeçalho para colocar no campo topo do headertemp 
    setTopo(headertemp, atualTopo);

    int m;
    char temp;
    REGISTRO *registrotemp;
    CRITERIOS *criterios;
    fseek(arqin, 17, SEEK_SET);
    for (int i=0; i< n; i++)
    {
        scanf("%d", &m);
        for (int j = 0; j < m; j++)
        {
        }
        fseek(arqin, 17, SEEK_SET); //antes de cada busca, devemos voltar ao primeiro registro pos-cabecalho
        while(fread(&temp, sizeof(char), 1, arqin))
        {
            fseek(arqin, -1, SEEK_CUR);
            registrotemp = recordFromBin(arqin);
            if (recordMeetsCriteria(registrotemp, m, criterios)) //verificamos se o registro bate com o criterio imposto pelo usuario
            {
                fseek(arqin, -80, SEEK_CUR); // se bater, entao voltamos 80 bytes (tamanho do registro) para modificar o campo "removido"
                int byteoffset = ftell(arqin); //vamos guardar a posicao do ponteiro atual, que preenchera o campo "topo" do cabeçalho
                int campoProximo; //esta variavel ira guardar o RRN que sera armazenado no campo "proximo" do registro
                atualTopo = getTopo(headertemp);
                if (atualTopo == -1) { //devemos tratar do caso em que este seja o primeiro registro a ser removido, pois o topo é -1 e essa posicao de byte offset nao existe
                    campoProximo = -1; //nesse caso, o campo "proximo" sera preenchido com -1 mesmo
                }
                else { //caso ja hajam outros registros removidos, devemos tratar o campo "proximo" como uma espécie de "pilha de RRN dos regristros removidos"
                    campoProximo = (atualTopo-17)/80; //pegamos o byte offset guardado no topo e traduzimos para um RRN
                }
                setTopo(headertemp, byteoffset);//o topo do cabecalho temporario agora é o byte offset do registro removido
                fwrite("1", sizeof(char), 1, arqin); //escrevemos 1 onde antes estava 0, simbolizando que o arquivo foi removido
                fwrite(&campoProximo, sizeof(int), 1, arqin); //escrevemos no campo "proximo" do registro o RRN daquele antigo topo do header 
                fseek(arqin, 75, SEEK_CUR); //este passo é necessario para irmos para o proximo registro; pulamos 75 bytes pois acabamos de escrever no campo "proximo", que termina no byte 4 (quinto byte)
                //ALTERAR NROESTACOES E NROPARESESTACAO
                //FAZER FUNÇÃO PARA ISSO
            }
        }
    }
    //volta pro começo do arquivo com fseek e printa tudo q nao tiver removido
}