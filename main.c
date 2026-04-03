#include <stdio.h>
#include <stdlib.h>
#include "fornecidas.h"
#include "funcionalidades.h"
#include "header.h"
#include "registros.h"

int main()
{
    int funcionalidade;
    char arquivoentrada[100]; 
    scanf("%d", &funcionalidade); //indica qual funcionalidade será usada
    scanf("%s", arquivoentrada); //indica o nome do arquivo que será usado como entrada 
    switch (funcionalidade)
    {
        case 1: //create table
            char arquivosaida[100];
            scanf("%s", arquivosaida); //nome do arquivo binário no qual será escrito pela função deve ser dado pelo usuário
            readRecords(arquivoentrada, arquivosaida);
            break;
        case 2: //mostrar registros
            showRecords(arquivoentrada);
            break;
        case 3: //mostrar registros com filtragem por campo
            int n;
            scanf("%d", &n);
            filterRecords(arquivoentrada, n);
            break;
        case 4: //remoção lógica
            int n;
            scanf("%d", &n);
            removeRecords(arquivoentrada, n);
            break;
        case 5: //inserção de registro
            int n;
            scanf("%d", &n);
            insertRecords(arquivoentrada, n);
            break;
        case 6: //atualização de arquivo
            int n;
            scanf("%d", &n);
            updateRecords(arquivoentrada, n);
            break;
        default: //funcionalidade inválida
            printf("Funcionalidade inválida!\n");
            break;
    }
    return 0;
}