#include <stdio.h>
#include <stdlib.h>
#include "fornecidas.h"
#include "funcionalidades.h"
#include "header.h"
#include "registros.h"

int main()
{
    int funcionalidade;
    char *arquivo; 
    scanf("%d", &funcionalidade); //indica qual funcionalidade será usada
    scanf("%s", arquivo); //indica o nome do arquivo que será usado como entrada 
    switch (funcionalidade)
    {
        case 1: //create table
            
            break;
        case 2: //mostrar registros

            break;
        case 3: //mostrar registros com filtragem por campo

            break;
        case 4: //remoção lógica

            break;
        case 5: //inserção de registro

            break;
        case 6: //atualização de arquivo

            break;
        default: //funcionalidade inválida
            printf("Funcionalidade inválida!\n");
            break;
    }
    return 0;
}