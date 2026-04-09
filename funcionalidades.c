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
        deleteHeader(header);
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
    
    changeHeaderStatus(header); //o arquivo será fechado, devo indicar isso com status = 1
    setNroEstacoes(header, contanomes); //atualizo o valor de header->nroEstacoes
    setNroParesEstacao(header, contapares); //atualizo o valor de header->nroParesEstacao
    
    fseek(arqout, 0, SEEK_SET); //coloco o ponteiro no inicio do arquivo
    writeHeaderOnBin(header, arqout); //sobrescrevo a header antiga com os novos dados da header
    deleteHeader(header);
    
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
    char removido; //indica se o registro está removido ou não
    REGISTRO *registrotemp;
    FILE *arqin = fopen(arqentrada, "rb"); //abertura do arquivo binário a ser lido
    if (arqin == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }
    fseek(arqin, 17, SEEK_SET); //ponteiro pula o cabeçalho, não será útil para a impressão dos registros
    while (fread(&removido, 1, 1, arqin)) //fread serve para verificar se ainda há registros a serem lidos
    {
        registrotemp = recordFromBin(arqin, removido); //crio registro temporário com os dados retirados do arquivo binário
        printRecord(registrotemp); //imprimo o registro se ele não tiver sido logicamente removido
        if (registrotemp != NULL)
        {
            deleteRecord(registrotemp);
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
    char removido;
    for (int i = 0; i < n; i++)
    {
        int achoualgum = 0;
        fseek(arqin, 17, SEEK_SET); //pulo o cabeçalho pra ir direto pro primeiro registro a cada busca
        scanf("%d", &m);
        CRITERIOS *criterios[m];
        readCriteria(m, criterios); //vetor de structs que vai armazenar cada um dos critérios de busca
        
        while(fread(&removido, sizeof(char), 1, arqin)) //verifica se o arquivo não acabou ainda
        {   
            registrotemp = recordFromBin(arqin, removido);//leio um registro 
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
        if(achoualgum == 0) printf("Registro inexistente.\n");
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
    HEADER *headertemp = headerFromBin(arqin); //vamos usar uma header temporaria para guardar as infos e adiciona-las ao header principal depois
    changeHeaderStatus(headertemp);
    int m;
    char removido;
    REGISTRO *registrotemp;
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
    contarEstacoesEPares(arqin, &nroEstacoes, &nroPares); //agora vamos recontar quantas estações unicas e pares de estações únicos nós temos
    setNroEstacoes(header, nroEstacoes); //colocamos o novo numero de estações na headertemp
    setNroParesEstacao(header, nroPares); //o mesmo para o numero de pares de estacao
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
                atualizarCamposRegistro(registrotemp, atts, criteriosAtt); //usamos essa funcao para armazenar as atualizações no registrotemp
                fseek(arqin, -80, SEEK_CUR); //vamos mover o curso para o byte offset do registro a ser atualizado
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
    return;
} 