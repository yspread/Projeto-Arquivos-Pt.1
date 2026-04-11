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
substituido por outro com os dados atualizados*/
void readRecords(char *arqentrada, char *arqsaida)
{
    FILE *arqin = fopen(arqentrada, "r");
    if (arqin == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }   
    FILE *arqout = fopen(arqsaida, "wb+");
    
    REGISTRO *registrotemp;
    char buffer[512]; //armazenará a linha que foi lida do csv
    
    HEADER *header = createHeader();
    setStatus(header, '0');
    writeHeaderOnBin(header, arqout); //escrita de uma header placeholder no arquivo binário

    fgets(buffer, 512, arqin); //primeira linha deve ser ignorada
    while (fgets(buffer, 512, arqin) != NULL){
        registrotemp = recordFromCSV(buffer);
        writeRecordOnBin(registrotemp, arqout);
        setProxRRN(header, (getProxRRN(header)+1)); //atualiza o campo proxRRN da header
        deleteRecord(registrotemp);
    }
    attHeaderOnFile(header, arqout);
    fclose(arqin);
    fclose(arqout);
    BinarioNaTela(arqsaida);
    return;
}

/*essa função vai percorrer por todo o arquivo binário de entrada
e imprimir todos os registros que não estão logicamente removidos
caso o arquivo não tenha registros não removidos, avisamos*/
void showRecords(char *arqentrada)
{
    FILE *arqin = fopen(arqentrada, "rb");
    if (arqin == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    int arqvazio = 1;
    REGISTRO *registrotemp;
    char removido;

    fseek(arqin, 17, SEEK_SET); //pula o cabeçalho
    while (fread(&removido, 1, 1, arqin)){
        registrotemp = recordFromBin(arqin, removido);
        printRecord(registrotemp);
        if (registrotemp != NULL)
        {
            arqvazio = 0; //como tinha pelo menos um registro != NULL, o arquivo não está vazio
            deleteRecord(registrotemp);
        }
    }

    if (arqvazio == 1){ //arquivo não possui registros, informamos ao usuário
        printf("Registro inexistente.\n");
    }

    fclose(arqin);
    return;
}

/*essa função vai receber como parametro um arquivo binário e vai fazer n buscas
por campo e valor do campo no arquivo. vai imprimir os resultados compatíveis
n equivale ao número de buscas diferentes que serão feitas e m equivale a quantidade de
campos que serão utilizados em cada busca
caso nenhum registro atenda os critérios, informamos*/
void filterRecords(char *arqentrada, int n)
{
    FILE *arqin = fopen(arqentrada, "rb");
    if (arqin == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }

    int m;
    REGISTRO *registrotemp;
    char removido;

    for (int i = 0; i < n; i++){
        scanf("%d", &m);
        CRITERIOS *criterios[m];//vetor de structs que vai armazenar cada um dos critérios de busca
        readCriteria(m, criterios); 

        int achoualgum = 0;
        fseek(arqin, 17, SEEK_SET);
        while(fread(&removido, sizeof(char), 1, arqin)){   
            registrotemp = recordFromBin(arqin, removido);
            if (registrotemp != NULL){
                if(recordMeetsCriteria(registrotemp, m, criterios)){ //verifica se o registro atende os criterios de busca
                    printRecord(registrotemp);
                    achoualgum = 1; //marcamos que pelo menos 1 registro compatível com a busca foi encontrado
                }
               deleteRecord(registrotemp);
            }
        }
        if(achoualgum == 0){
            printf("Registro inexistente.\n");
        }
        printf("\n");
        
        for(int j = 0; j < m; j++){
            deleteCriteria(criterios[j]);
        }
    }
    fclose(arqin);
}

/*essa funcao vai remover um registro de acordo com uma especificação (criterio) do usuario, semelhante a funcionalidade 3.
Os registros removidos formarão uma pilha, e o topo dessa pilha estará armazenado na header.
Em cada registro, o campo "proximo" armazena o rrn do proximo registro na pilha, para auxiliar no desempilhamento quando necessário*/
void removeRecords(char *arqentrada, int n)
{
    FILE *arqin = fopen(arqentrada, "rb+");
    if (arqin == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }
    char stts = '0';
    fwrite(&stts, sizeof(char), 1, arqin); //antes de criar a header auxiliar, marco q o arquivo está inconsistente
    fseek(arqin, 0, SEEK_SET);
    HEADER *headertemp = headerFromBin(arqin);
    
    int m;
    REGISTRO *registrotemp;
    char removido;

    for (int i=0; i< n; i++){
        scanf("%d", &m);
        CRITERIOS *criteriosbusca[m];
        readCriteria(m, criteriosbusca);
        
        fseek(arqin, 17, SEEK_SET); //ponteiro volta pro início para fazer a próxima remoção
        int countrrn = 0; //conta o rrn do registro em que estamos operando
        while(fread(&removido, sizeof(char), 1, arqin)){
            registrotemp = recordFromBin(arqin, removido);
            if (registrotemp != NULL){
                if (recordMeetsCriteria(registrotemp, m, criteriosbusca)){
                    setProximo(registrotemp, getTopo(headertemp));//o campo "proximo" do registro removido recebe o rrn do antigo topo da pilha
                    setTopo(headertemp, countrrn); //registro que acabou de ser removido é o novo topo da pilha
                    removeRecord(registrotemp);
                    fseek(arqin, -80, SEEK_CUR);  //voltamos para o inicio do registro e gravamos o registro atualizado
                    writeRecordOnBin(registrotemp, arqin);
                }
                deleteRecord(registrotemp);
            }
            countrrn++;
        }

        for (int j = 0; j < m; j++) { 
            deleteCriteria(criteriosbusca[j]); 
        }
    }
    attHeaderOnFile(headertemp, arqin);
    fclose(arqin);
    BinarioNaTela(arqentrada);
    return;
}

/*esta é uma função para inserir novos registros no arquivo. Basicamente vamos sempre procurar pelo
proximo espaço vazio do arquivo: se nao há arquivos removidos ainda, esse espaço será o proxRRN do cabeçalho,
mas se ja houverem registros removidos, devemos realizar a inserção com base na pilha de registros removidos,
sempre inserindo o registro no topo da pilha*/
void insertRecords(char *arqentrada, int n)
{
    FILE *arqin = fopen(arqentrada, "rb+");
    if (arqin == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }
    //marcamos o arquivo como inconsistente antes de ler a header
    char stts = '0';
    fwrite(&stts, sizeof(char), 1, arqin);
    fseek(arqin, 0, SEEK_SET);
    HEADER *header = headerFromBin(arqin);

    REGISTRO *registrotemp;

    for (int i=0; i < n; i++) {
        registrotemp = recordFromInput();
        if (getTopo(header) == -1) { //se nao ha topo, entao nao ha registros removidos e insercao ocorre no byte offset do proxRRN
            int byteoffsetproxRRN = getProxRRN(header) * 80 + 17; //devemos achar o byte offset do proxRRN para fazer a insercao
            fseek(arqin, byteoffsetproxRRN, SEEK_SET);
            writeRecordOnBin(registrotemp, arqin); //escrevemos na posição indicada
            setProxRRN(header, getProxRRN(header) + 1);
        }
        else { //como topo != -1, devemos realizar as inserções de acordo com a pilha de registros removidos, desempilhando-a
            int byteoffsettopo = getTopo(header) * 80 + 17;
            int novoTopo; 
            fseek(arqin, byteoffsettopo+1, SEEK_SET); 
            fread(&novoTopo, sizeof(int), 1, arqin); //lemos o campo "proximo" do antigo topo da pilha. Ele será o RRN do novo topo
            setTopo(header, novoTopo);

            fseek(arqin, byteoffsettopo, SEEK_SET); //escrevemos o registro inserido no topo da pilha
            writeRecordOnBin(registrotemp, arqin);
        }
        deleteRecord(registrotemp);
    }
    attHeaderOnFile(header, arqin);
    fclose(arqin);
    BinarioNaTela(arqentrada);
    return;
}

/*esta é uma função para atualizar campos de um registro. o usuário insere primeiro os critérios para se buscar
o registro a ser atualizado e em seguida insere os valores e os campos a serem atualizados
assim, sobreescrevemos o registro com os novos dados
serão feitas n atualizações de registros, conforme informado pelo usuário*/
void updateRecords(char *arqentrada, int n) {
    FILE *arqin = fopen(arqentrada, "rb+");
    if (arqin == NULL){
        printf("Falha no processamento do arquivo.");
        return;
    }
    //marcamos o arquivo como inconsistente
    char stts = '0';
    fwrite(&stts, sizeof(char),1,arqin);
    fseek(arqin, 0, SEEK_SET);
    HEADER *headertemp = headerFromBin(arqin);

    int m, atts;
    REGISTRO *registrotemp;
    char removido;

    for (int i=0; i< n; i++){
        //criterios de busca
        scanf("%d", &m);
        CRITERIOS *criteriosbusca[m];
        readCriteria(m, criteriosbusca); 
        
        //criterios de atualização
        scanf("%d", &atts);
        CRITERIOS *criteriosatt[m];
        readCriteria(atts, criteriosatt);

        fseek(arqin, 17, SEEK_SET); 
        while(fread(&removido, sizeof(char), 1, arqin)){
            registrotemp = recordFromBin(arqin, removido);
            if (registrotemp != NULL){
                if (recordMeetsCriteria(registrotemp, m, criteriosbusca)) {
                    attRecords(registrotemp, atts, criteriosatt); //atualizamos o registro encontrado com os novos valores inseridos pelo usuário
                    fseek(arqin, -80, SEEK_CUR); //voltamos para poder sobreescrever o registro com os novos dados
                    writeRecordOnBin(registrotemp, arqin);
                }
                deleteRecord(registrotemp);
            }
        }

        for (int j = 0; j < atts; j++) {
            deleteCriteria(criteriosatt[j]);
        }
        for (int j = 0; j < m; j++) {
            deleteCriteria(criteriosbusca[j]); 
        }
    }
    attHeaderOnFile(headertemp, arqin);
    fclose(arqin);
    BinarioNaTela(arqentrada);
    return;
} 