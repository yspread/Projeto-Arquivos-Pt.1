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
    REGISTRO *registrotemp;
    char buffer[512]; //armazenará a linha que foi lida do csv
    
    FILE *arqin = fopen(arqentrada, "r");
    if (arqin == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }   
    FILE *arqout = fopen(arqsaida, "wb+");
    
    HEADER *header = createHeader();
    changeHeaderStatus(header);
    writeHeaderOnBin(header, arqout); //escrita de uma header placeholder no arquivo binário

    fgets(buffer, 512, arqin); //primeira linha deve ser ignorada
    while (fgets(buffer, 512, arqin) != NULL){
        registrotemp = recordFromCSV(buffer);
        writeRecordOnBin(registrotemp, arqout);
        setProxRRN(header, (getProxRRN(header)+1)); //atualiza o campo proxRRN da header
        deleteRecord(registrotemp);
    }

    //atualizando os campos da header
    attCountersHeader(arqout, header);
    changeHeaderStatus(header);
    
    fseek(arqout, 0, SEEK_SET); //volta pro começo do arquivo binário para poder escrever a header atualizada
    writeHeaderOnBin(header, arqout);
    deleteHeader(header);
    
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
    int arqvazio = 1;
    char removido;
    REGISTRO *registrotemp;

    FILE *arqin = fopen(arqentrada, "rb");
    if (arqin == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

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

/*essa funcao vai remover um registro de acordo com uma especificação (criterio) do usuario, usando a funcionalidade 3
os registros removidos vao compor uma pilha para guardar os endereços e colocar novos registros
nesses endereços */
void removeRecords(char *arqentrada, int n)  {
    FILE *arqin = fopen(arqentrada, "rb+"); //vamos abrir o arquivo em wb para ler e escrever
    if (arqin == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }
    HEADER *headertemp = headerFromBin(arqin); //vamos usar uma header temporaria para guardar as infos e adiciona-las ao header principal depois
    changeHeaderStatus(headertemp);
    int m;
    REGISTRO *registrotemp;
    char removido;
    for (int i=0; i< n; i++)
    {
        scanf("%d", &m);
        CRITERIOS *criteriosBusca[m];
        readCriteria(m, criteriosBusca);

        fseek(arqin, 17, SEEK_SET); //antes de cada busca, devemos voltar ao primeiro registro pos-cabecalho
        int countrrn = 0; //conta o rrn do registro em que estamos operando
        while(fread(&removido, sizeof(char), 1, arqin))
        {
            //int byteOffSetRegistro = ftell(arqin); //guardemos o byteoffset do registro
            registrotemp = recordFromBin(arqin, removido);
            if (registrotemp != NULL && recordMeetsCriteria(registrotemp, m, criteriosBusca)) //verificamos se o registro bate com o criterio imposto pelo usuario
            {
                //int rrnAtual = (byteOffSetRegistro - 17) / 80; //calculamos o rrn do byte offset do registro em questao
                setProximo(registrotemp, getTopo(headertemp));//o campo "proximo" do registro removido recebe o rrn do antigo topo da pilha
                //setTopo(headertemp, rrnAtual); //atualizamos o topo do cabecalho
                setTopo(headertemp, countrrn);
                removeRecord(registrotemp);
                fseek(arqin, -80, SEEK_CUR);  //voltamos para o inicio do registro e gravamos o registro atualizado
                writeRecordOnBin(registrotemp, arqin); 
            }
            countrrn++;
            if (registrotemp != NULL) { //podemos nos livrar do registrotemp
                deleteRecord(registrotemp);
            }
        }

        for (int j = 0; j < m; j++) { 
            deleteCriteria(criteriosBusca[j]); 
        }
    }
    int nroEstacoes, nroPares;
    attCountersHeader(arqin, headertemp); //agora vamos recontar quantas estações unicas e pares de estações únicos nós temos
    changeHeaderStatus(headertemp);
    fseek(arqin, 0, SEEK_SET); //hora de atualizar a header principal 
    writeHeaderOnBin(headertemp, arqin); //escrevemos a header temporaria na header principal
    deleteHeader(headertemp); //agora podemos liberar a memoria da headertemp
    fclose(arqin);
    BinarioNaTela(arqentrada);
}

/*esta é uma função para inserir novos registros no arquivo. Basicamente vamos sempre procurar pelo
proximo espaço vazio do arquivo: se nao há arquivos removidos ainda, esse espaço será o proxRRN do cabeçalho,
mas se ja houverem registros removidos, devemos escrever os novos registros no espaço liberado pelos registros removidos.
Podemos achar esses espacos por meio da pilha de registros logicamente removidos */
void insertRecords(char *arqentrada, int n) {
    FILE *arqin = fopen(arqentrada, "rb+"); //vamos abrir o arquivo em wb para ler e escrever
    if (arqin == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }
    //vamos usar a mesma tecnica de header temporaria usada na funcionalidade 4 (removeRecords) para guaradar o topo e o proxRRN
    HEADER *header = headerFromBin(arqin);
    changeHeaderStatus(header);
    REGISTRO *registrotemp;
    for (int i=0; i < n; i++) { //loop que escaneia e insere n vezes
        registrotemp = recordFromInput();

        if (getTopo(header) == -1) { //se nao ha topo, entao nao ha registross removidos.A insercao ocorre no byte offset do proxRRN
            int byteOffSetproxRRN = getProxRRN(header) * 80 + 17; //devemos achar o byte offset do proxRRN para fazer a insercao
            fseek(arqin, byteOffSetproxRRN, SEEK_SET); //movemos o cursor para la
            writeRecordOnBin(registrotemp, arqin);
            setProxRRN(header, getProxRRN(header) + 1); //como inserimos no fim do arquivo, devemos incrementar o campo proxRRN do cabeçalho 
        }
        else { //se topo !=1, entao ha registros removidos. Devemos usar a pilha de registros removidos para achar esses espaços vazios e inserir ali
            //como a pilha se da pelo campo "proximo" dos registros, devemos pegar o "proximo" do topo, tornar esse proximo o novo topo e 
            //escrever o novo registro no antigo topo
            int byteOffsetBuraco = getTopo(header) * 80 + 17; //vamos guardar o byte offset do "buraco" de memoria
            int novoTopo; //o campo "proximo" do antigo topo da pilha vai ser o novo topo
            fseek(arqin, byteOffsetBuraco+1, SEEK_SET); //movemos o cursor para a posicao do campo "proximo" do topo
            fread(&novoTopo, sizeof(int), 1, arqin); //lemos o RRN que estava ali e guardamos na variavel proxAntigoTopo
            setTopo(header, novoTopo); //esse byte offset é o novo topo da pilha. Botamos ele no campo "topo" da header temporaria
            fseek(arqin, byteOffsetBuraco, SEEK_SET); //aqui botamos o cursor no byte offset do antigo topo da pilha de removidos. É aqui que vamos escrever o novo registro
            //escreverNoRegistro(arqin, codEstacao, codLinha, codProxEstacao, distProxEstacao, codLinhaIntegra, codEstIntegra, nomeEstacao, nomeLinha) ;
            writeRecordOnBin(registrotemp, arqin);
        }
    }
    int nroEstacoes, nroPares;
    fseek(arqin, 17, SEEK_SET);
    attCountersHeader(arqin, header); //agora vamos recontar quantas estações unicas e pares de estações únicos nós temos
    changeHeaderStatus(header);
    fseek(arqin, 0, SEEK_SET); //hora de atualizar a header principal 
    writeHeaderOnBin(header, arqin); //escrevemos a header temporaria na header principal
    
    deleteHeader(header); //agora podemos liberar a memoria da headertemp
    fclose(arqin);
    BinarioNaTela(arqentrada);
    return;
}

/*esta é uma função para atualizar um registro, caso ele exista segundo criterios definidos pelo usuario*/
void updateRecords(char *arqentrada, int n) {
    FILE *arqin = fopen(arqentrada, "rb+"); //vamos abrir o arquivo em wb para ler e escrever
    if (arqin == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }
    fwrite("1", sizeof(char),1,arqin);
    int m;
    char removido;
    REGISTRO *registrotemp;
    for (int i=0; i< n; i++)
    {
        scanf("%d", &m);
        CRITERIOS *criteriosBusca[m];
        readCriteria(m, criteriosBusca); 

        int atts; //numero de atualizacoes para aquela linha (o "p" no exemplo do pdf)
        scanf("%d", &atts);  
        CRITERIOS *criteriosAtt[m];
        readCriteria(atts, criteriosAtt); //vamos guardar as atualizacoes aqui

        fseek(arqin, 17, SEEK_SET); //antes de cada busca, devemos voltar ao primeiro registro pos-cabecalho
        while(fread(&removido, sizeof(char), 1, arqin))
        {
            //int byteOffSetRegistro = ftell(arqin); //vamos guardar este byte offset para caso este seja o registro que atualizaremos
            registrotemp = recordFromBin(arqin, removido); //guardaremos o registro lido em registrotemp (que sera usado para ver se os criterios batem)
            if (registrotemp != NULL && recordMeetsCriteria(registrotemp, m, criteriosBusca)) { //verificamos se o registro bate com o criterio imposto pelo usuario
                attRecords(registrotemp, atts, criteriosAtt); //usamos essa funcao para armazenar as atualizações no registrotemp
                fseek(arqin, -80, SEEK_CUR); //vamos mover o curso para o byte offset do registro a ser atualizado
                writeRecordOnBin(registrotemp, arqin); //escrevemos o registro atualizado ali
            }
            if (registrotemp != NULL) //desaloco a memória alocada para o registro temporário
            {
                deleteRecord(registrotemp);
            }
        }
        for (int j = 0; j < atts; j++) {
            deleteCriteria(criteriosAtt[j]);
        }

        for (int j = 0; j < m; j++) { //vamos liberar a memoria dos criterios antes da proxima volta do loop
            deleteCriteria(criteriosBusca[j]); 
        }
    }
    fclose(arqin);

    BinarioNaTela(arqentrada);
    return;
} 