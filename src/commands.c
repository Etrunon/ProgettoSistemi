#include "CONST.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "commands.h"
#include "logica.h"

/*Cerca di analizzare l'input
 * L'utente può aver inserito o un numero come risposta
 * Oppure il suo nome, per presentarsi al server
 */
comando analizzaInput(data* output, char* input) {
    char* carattereErrore = NULL;
    int numero;
    numero = strtol(input, &carattereErrore, 10);
    //se ho un numero, controllo se è nel range ammissibile per essere inviato al server
    if (*carattereErrore == '\0') {
        if (numero > 999999 || numero < -99999) {
            printf("%s", ANSI_COLOR_RED "Risposta non ammissibile" ANSI_COLOR_RESET);
            return ERRORE;
        }
        output->risposta = numero;
        return RISPOSTA;
    } else {
        strcpy(output->nome, input);
        return NOME;
    }
    return ERRORE;
}

comando leggiInput(bool server, data* inputUtente) {
    char* commands = NULL;
    size_t size;
    char command [MAXCOMMAND];

    /*Leggo l'intera linea*/
    fflush(stdin);
    int read = getline(&commands, &size, stdin);

    commands[read - 1] = '\0';
    /*Considero solo il primo comando inviato nell'intera linea*/
    if (strlen(commands) > MAXCOMMAND) {
        printf("%s\n", ANSI_COLOR_RED "Input non valido" ANSI_COLOR_RESET);
        return ERRORE;
    }
    sscanf(commands, "%s", command);
    free(commands);

    /*Conrollo che non abbia stringhe troppo lunghe in input*/

    if ((strcasecmp(command, "exit")) == 0) {
        return CHIUSURA;
    }
    if ((strcasecmp(command, "classifica")) == 0) {
        return CLASSIFICA;
    }
    if ((strcasecmp(command, "help")) == 0) {
        return HELP;
    }
    /*Se sono interessato ad input numerici o testuali, chiamo la funzione apposita*/
    if (inputUtente != NULL)
        return analizzaInput(inputUtente, command);
    else
        return ERRORE;
}

void printHelp(bool server) {
    printf(ANSI_COLOR_GREEN);
    printf("%45s", "----------------------------------------");
    printf("\n%30s\n", "Comandi disponibili");
    printf("%-15s%s\n", "help", "Visualizza questo messaggio");
    printf("%-15s%s\n", "exit", "Esci dalla partita");
    if (!server) {
        printf("%-15s%s\n", "classifica", "Stampa la classifica di giocatori");
    }
    printf("%45s\n", "----------------------------------------");
    printf(ANSI_COLOR_RESET);
}