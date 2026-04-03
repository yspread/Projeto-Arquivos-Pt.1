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
    char *listanomesestacoes[10000]; //vai armazenar os nomes de estacoes que ja foram registrados, a fim de não contar os repetidos
    int contanomes = 0; //conta quantos nomes diferentes tem na lista   
    char *nomeestacaotemp; //armazenará temporariamente o nome da estacao de um registro
    int listacodestacoes[10000], listacodproxestacoes[10000]; //lista vai armazenar os pares de codEstacao e codProxEstacao diferentes
    int contapares = 0; //conta quantos pares codEstacao, codProxEstaco diferentes tem na lista
    int codestacaotemp, codproxestacaotemp;; //armazenarao temporariamente o codigo da estacao e o codigo da proxima estacao de um registro
    HEADER *header = createHeader(); //crio uma header que será preenchida conforme os registros são lidos
    REGISTRO *registrotemp; //esse registro temporário armazenará o registro que vai ser escrito no binário
    char buffer[512]; //armazenará a linha que foi lida do csv
    FILE *arqin = fopen(arqentrada, "r"); //abertura do arquivo de entrada para leitura
    if (arqin == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }
    FILE *arqout = fopen(arqsaida, "wb"); //abertura do arquivo de saida para escrita em binário
    changeHeaderStatus(header); //o arquivo foi aberto para escrita, status atualizado
    writeHeaderOnBin(header, arqout); //escrita dos 17 bytes do cabeçalho inicial que será posteriormente sobrescrito(posso escrever a struct inteira de uma vez, visto que todos os campos tem tamanho fixo)
    fgets(buffer, 512, arqin); //primeira linha deve ser ignorada
    while (fgets(buffer, 512, arqin) != NULL) //enquanto não acaba o arquivo
    {
        registrotemp = recordFromCSV(buffer); //crio registro temporário com os valores presentes na linha
        writeRecordOnBin(registrotemp, arqout); //escrevo os 80 bytes do registro no arquivo binário
        setProxRRN(header, (getProxRRN(header)+1));
        /*descubro se o nome da estacao do registro atual é novo, se for, atualizo header->nroEstacoes, contando + 1*/
        nomeestacaotemp = getNomeEstacao(registrotemp);
        if (contanomes == 0) 
        {
            listanomesestacoes[contanomes] = strdup(nomeestacaotemp);
            contanomes++;
        }
        else
        {   
            for(int i = 0; i < contanomes; i++)
            {   
                if(strcmp(listanomesestacoes[i], nomeestacaotemp) == 0) //se eu achar algum nome igual na lista, nao conto como nome novo de estacao
                {
                    break;
                }
                if(i == (contanomes - 1)) //se chegou no final da lista e não achou nenhum nome igual, eu conto++ e adiciono o novo nome na lista
                {
                    listanomesestacoes[contanomes] = strdup(nomeestacaotemp);
                    contanomes++;
                }
            }
        }
        /*faço a mesma coisa que com o numero de com o nome da estacao, so que agora com o par (codEstacao, codProxEstacao)
        se for um par novo, conto + 1 em header->nroParesEstacao*/
        codestacaotemp = getCodEstacao(registrotemp);
        codproxestacaotemp = getCodProxEstacao(registrotemp);
        if (contapares == 0 && codproxestacaotemp != -1) //essa estação não pode ser terminal para ser contada como par
        {
            listacodestacoes[contapares] = codestacaotemp;
            listacodproxestacoes[contapares] = codproxestacaotemp;
            contapares++;
        }
        else
        {
            for(int i = 0; i < contapares; i++)
            {
                if(codproxestacaotemp == -1 || ((listacodestacoes[i] == codestacaotemp && listacodproxestacoes[i] == codproxestacaotemp) || listacodestacoes[i] == codproxestacaotemp && listacodproxestacoes[i] == codestacaotemp)) //se eu achar um par igual na lista, nao conto como par (estações terminais são desconsideradas)
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
        }
        deleteRecord(registrotemp);
    }
    if (contanomes != 0)
    {
        for(int i = 0; i < contanomes; i++)
        {
            free(listanomesestacoes[i]); //libero a memória alocada as strings pela função strdup
            listanomesestacoes[i] = NULL;
        }
    }
    fseek(arqout, 0, SEEK_SET); //coloco o ponteiro no inicio do arquivo
    changeHeaderStatus(header); //o arquivo será fechado, devo indicar isso com status = 1
    setNroEstacoes(header, contanomes); //atualizo o valor de header->nroEstacoes
    setNroParesEstacao(header, contapares); //atualizo o valor de header->nroParesEstacao
    writeHeaderOnBin(header, arqout); //sobrescrevo a header antiga com os novos dados da header
    fclose(arqin);
    fclose(arqout);
    BinarioNaTela(arqsaida);
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
        printf("Falha no processamento do arquivo.");
        return;
    }
    fseek(arqin, 17, SEEK_SET); //ponteiro pula o cabeçalho, não será útil para a impressão dos registros
    while (fread(&temp, 1, 1, arqin)) //fread serve para verificar se ainda há registros a serem lidos
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
        printf("Falha no processamento do arquivo.");
        return;
    }
    int m;
    REGISTRO *registrotemp;
    char nomecampo[256];
    char valorcampo[256];
    char temp;
    for (int i = 0; i < n; i++)
    {
        int achoualgum = 0;
        fseek(arqin, 17, SEEK_SET); //pulo o cabeçalho pra ir direto pro primeiro registro a cada busca
        scanf("%d", &m);
        CRITERIOS *criterios[m]; //vetor de structs que vai armazenar cada um dos critérios de busca
        for (int j = 0; j < m; j++)
        {
            scanf("%s", nomecampo);
            if (strcmp(nomecampo, "nomeEstacao") == 0 || strcmp(nomecampo, "nomeLinha") == 0)
            {
                ScanQuoteString(valorcampo);
            }
            else
            {
                scanf("%s", valorcampo);
            }
            criterios[j] = createCriteria(nomecampo, valorcampo);
        }
        while(fread(&temp, sizeof(char), 1, arqin)) //verifica se o arquivo não acabou ainda
        {   
            fseek(arqin, -1, SEEK_CUR);
            registrotemp = recordFromBin(arqin);//leio um registro 
            if (registrotemp != NULL && recordMeetsCriteria(registrotemp, m, criterios))//se o registro atender aos criterios de busca e não estiver logicamente removido ele é printado
            {
                printRecord(registrotemp);
                achoualgum = 1; //achou algum portanto não será necessário imprimir aviso de que nenhum registro é compativel com a busca
            }
            if (registrotemp != NULL) //desaloco a memória alocada para o registro temporário
            {
                deleteRecord(registrotemp);
            }
        }
        for(int j = 0; j < m; j++)
        {
            deleteCriteria(criterios[j]);
        }
        if (achoualgum == 0)
        {
            printf("Registro inexistente.\n");
        }
        printf("\n");
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
    HEADER *headertemp = createHeader(); //vamos usar uma header temporaria para guardar as infos e adiciona-las ao header principal depois
    changeHeaderStatus(headertemp); 
    fseek(arqin, 1, SEEK_SET); //vamos para o campo topo do cabeçalho
    int atualTopo; 
    fread(&atualTopo, sizeof(int), 1, arqin); //vamos armazenar o atual topo do cabeçalho para colocar no campo topo do headertemp 
    setTopo(headertemp, atualTopo);

    int m;
    char temp;
    char nomecampo[256];
    char valorcampo[256];
    char *stringtemp;
    REGISTRO *registrotemp;
    fseek(arqin, 17, SEEK_SET);
    for (int i=0; i< n; i++)
    {
        scanf("%d", &m);
        CRITERIOS *criteriosBusca[m];
        for (int j = 0; j < m; j++)
        {
            scanf("%s", nomecampo); //vamos ler o nome do campo que vamos atualizar
            if (strcmp(nomecampo, "nomeEstacao") == 0 || strcmp(nomecampo, "nomeLinha") == 0){ //se esse campo for nomeEstacao ou nomeLinha, ele esta entre aspas
                ScanQuoteString(valorcampo);
            }
            else { //se nao for, nao está entre aspas e usamos scanf normal
                scanf("%s", valorcampo);
            }
            criteriosBusca[j] = createCriteria(nomecampo, valorcampo);
        }
        fseek(arqin, 17, SEEK_SET); //antes de cada busca, devemos voltar ao primeiro registro pos-cabecalho
        while(fread(&temp, sizeof(char), 1, arqin))
        {
            fseek(arqin, -1, SEEK_CUR);
            registrotemp = recordFromBin(arqin);
            if (recordMeetsCriteria(registrotemp, m, criteriosBusca)) //verificamos se o registro bate com o criterio imposto pelo usuario
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
                int numUm;
                fwrite(&numUm, sizeof(char), 1, arqin); //escrevemos 1 onde antes estava 0, simbolizando que o arquivo foi removido
                fwrite(&campoProximo, sizeof(int), 1, arqin); //escrevemos no campo "proximo" do registro o RRN daquele antigo topo do header 
                fseek(arqin, 75, SEEK_CUR); //este passo é necessario para irmos para o proximo registro; pulamos 75 bytes pois acabamos de escrever no campo "proximo", que termina no byte 4 (quinto byte)
            }
        }
    }
    int nroEstacoes, nroPares;
    contarEstacoesEPares(arqin, &nroEstacoes, &nroPares); //agora vamos recontar quantas estações unicas e pares de estações únicos nós temos
    setNroEstacoes(headertemp, nroEstacoes); //colocamos o novo numero de estações na headertemp
    setNroParesEstacao(headertemp, nroPares); //o mesmo para o numero de pares de estacao
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
    HEADER *headertemp = createHeader(); //vamos usar uma header temporaria para guardar as infos e adiciona-las ao header principal depois
    changeHeaderStatus(headertemp); 
    fseek(arqin, 1, SEEK_SET); //vamos para o campo topo do cabeçalho
    int atualTopo, atualproxRRN; 
    fread(&atualTopo, sizeof(int), 1, arqin); //vamos armazenar o atual topo do cabeçalho para colocar no campo topo do headertemp 
    fread(&atualproxRRN, sizeof(int), 1, arqin); //vamos fazer o mesmo com o proxRRN. No fim da função vamos botar isso tudo na header principal
    setTopo(headertemp, atualTopo);
    setProxRRN(headertemp, atualproxRRN);

    for (int i=0; i< n; i++) { //loop que escaneia e insere n vezes
        char codEstacao[50];
        char codLinha[50];
        char codProxEstacao[50];
        char distProxEstacao[50];
        char codLinhaIntegra[50];
        char codEstIntegra[50];
        char nomeEstacao[50];
        char nomeLinha[50];
        scanf("%s", &codEstacao); //temos que fazer o scanf na ordem exemplificada nas especificacoes
        scanf("%s", &codLinha); 
        scanf("%s", &codProxEstacao);
        scanf("%s", &distProxEstacao);
        scanf("%s", &codLinhaIntegra);
        scanf("%s", &codEstIntegra);
        ScanQuoteString(nomeEstacao); //para as strings, que estao entre aspas, usamos a funcao fornecida
        ScanQuoteString(nomeLinha);

        if (atualTopo == -1) { //se nao ha topo, entao nao ha registross removidos.A insercao ocorre no byte offset do proxRRN
            int byteOffSetproxRRN = atualproxRRN * 80 + 17; //devemos achar o byte offset do proxRRN para fazer a insercao
            fseek(arqin, byteOffSetproxRRN, SEEK_SET); //movemos o cursor para la
            escreverNoRegistro(arqin, codEstacao, codLinha, codProxEstacao, distProxEstacao, codLinhaIntegra, codEstIntegra, nomeEstacao, nomeLinha);


            setProxRRN(headertemp, getProxRRN(headertemp) + 1); //como inserimos no fim do arquivo, devemos incrementar o campo proxRRN do cabeçalho 
        }
        else { //se topo !=1, entao ha registros removidos. Devemos usar a pilha de registros removidos para achar esses espaços vazios e inserir ali
            //como a pilha se da pelo campo "proximo" dos registros, devemos pegar o "proximo" do topo, tornar esse proximo o novo topo e 
            //escrever o novo registro no antigo topo
            int proxAntigoTopo; //vamos guardar nesta variavel o campo "proximo" daquele que é o atual (mas logo será o antigo) topo da pilha de removidos
            fseek(arqin, atualTopo+1, SEEK_SET); //movemos o cursor para a posicao do campo "proximo" do topo
            fread(&proxAntigoTopo, sizeof(int), 1, arqin); //lemos o RRN que estava ali e guardamos na variavel proxAntigoTopo
            int novoTopo = proxAntigoTopo * 80 + 17; //trnasformamos aquele RRN em byte offset
            setTopo(headertemp, novoTopo); //esse byte offset é o novo topo da pilha. Botamos ele no campo "topo" da header temporaria
            fseek(arqin, atualTopo, SEEK_SET); //aqui botamos o cursor no byte offset do antigo topo da pilha de removidos. É aqui que vamos escrever o novo registro
            escreverNoRegistro(arqin, codEstacao, codLinha, codProxEstacao, distProxEstacao, codLinhaIntegra, codEstIntegra, nomeEstacao, nomeLinha) ;
        }
    }
    fseek(arqin, 1, SEEK_SET); //hora de atualizar a header principal 
    int novoTopo = getTopo(headertemp); //vamos pegar o topo da headertemp (o atual topo da pilha)
    int novoProxRRN = getProxRRN(headertemp); //o mesmo vale para o proxRRN
    fwrite(&novoTopo, sizeof(int), 1, arqin); //e escrever esse novo atual topo na header principal
    fwrite(&novoProxRRN, sizeof(int), 1, arqin); //e escrever esse novo atual topo na header principal
    deleteHeader(headertemp); //agora podemos liberar a memoria da headertemp
    fclose(arqin);
    BinarioNaTela(arqentrada);
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
    char nomecampo[256];
    char valorcampo[256];
    char temp;
    char *stringtemp;
    REGISTRO *registrotemp;
    fseek(arqin, 17, SEEK_SET);
    for (int i=0; i< n; i++)
    {
        scanf("%d", &m);
        CRITERIOS *criteriosBusca[m]; 
        for (int j = 0; j < m; j++)
        {
            scanf("%s", nomecampo); //vamos ler o nome do campo que vamos atualizar
            if (strcmp(nomecampo, "nomeEstacao") == 0 || strcmp(nomecampo, "nomeLinha") == 0){ //se esse campo for nomeEstacao ou nomeLinha, ele esta entre aspas
                ScanQuoteString(valorcampo);
            }
            else { //se nao for, nao está entre aspas e usamos scanf normal
                scanf("%s", valorcampo);
            }
            criteriosBusca[j] = createCriteria(nomecampo, valorcampo);
        }
        int atts; //numero de atualizacoes para aquela linha (o "p" no exemplo do pdf)
        scanf("%d", &atts);  
        CRITERIOS *criteriosAtt[atts]; //vamos guardar as atualizacoes aqui
        for (int j = 0; j < atts; j++)  //vamos guardar uma atualizacao para cada att (numero de atualizacoes)
        {
            scanf("%s", nomecampo); //vamos ler o nome do campo que vamos atualizar
            if (strcmp(nomecampo, "nomeEstacao") == 0 || strcmp(nomecampo, "nomeLinha") == 0){ //se esse campo for nomeEstacao ou nomeLinha, ele esta entre aspas
                ScanQuoteString(valorcampo);
            }
            else { //se nao for, nao está entre aspas e usamos scanf normal
                scanf("%s", valorcampo);
            }
            criteriosAtt[j] = createCriteria(nomecampo, valorcampo);
        }

        fseek(arqin, 17, SEEK_SET); //antes de cada busca, devemos voltar ao primeiro registro pos-cabecalho
        while(fread(&temp, sizeof(char), 1, arqin))
        {
            fseek(arqin, -1, SEEK_CUR);
            int byteOffSetRegistro = ftell(arqin); //vamos guardar este byte offset para caso este seja o registro que atualizaremos
            registrotemp = recordFromBin(arqin); //guardaremos o registro lido em registrotemp (que sera usado para ver se os criterios batem)
            if (recordMeetsCriteria(registrotemp, m, criteriosBusca)) { //verificamos se o registro bate com o criterio imposto pelo usuario
                atualizarCamposRegistro(registrotemp, atts, criteriosAtt); //usamos essa funcao para armazenar as atualizações no registrotemp
                fseek(arqin, byteOffSetRegistro, SEEK_SET); //vamos mover o curso para o byte offset do registro a ser atualizado
                writeRecordOnBin(registrotemp, arqin); //escrevemos o registro atualizado ali
            }
            if (registrotemp != NULL) //desaloco a memória alocada para o registro temporário
            {
                deleteRecord(registrotemp);
            }
        }
        for (int j = 0; j < m; j++) { //vamos liberar a memoria dos criterios antes da proxima volta do loop
            deleteCriteria(criteriosBusca[j]); 
        }
        for (int j = 0; j < atts; j++) {
            deleteCriteria(criteriosAtt[j]);
        }
    }
    fclose(arqin);
    BinarioNaTela(arqentrada);
}