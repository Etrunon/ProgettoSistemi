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
            printf("%s", "Risposta non ammissibile");
            return ERRORE;
        }
        output->risposta = numero;
        return RISPOSTA;
    } else {
        /*L'utente ha inserito un nome*/
        strcpy(output->nome, input);
        return NOME;
    }
    return ERRORE;
}

comando leggiInput(bool server, data* inputUtente) {
    char* commands = NULL;
    size_t size;
    char command [MAXCOMMAND];

    if (server) {
        printf("%s", "Server:");
    } else {
        printf("%s", "Client:");
    }
    /*Leggo l'intera linea*/
    int read = getline(&commands, &size, stdin);

    commands[read - 1] = '\0';
    free(commands);
    /*Considero solo il primo comando inviato nell'intera linea*/
    sscanf(commands, "%s", command);

    /*Conrollo che non abbia stringhe troppo lunghe in input*/
    if (strlen(command) > MAXCOMMAND) {
        printf("%s\n", "Input non valido");
        return ERRORE;
    }

    if ((strcasecmp(command, "exit")) == 0) {
        return CHIUSURA;
    }
    if ((strcasecmp(command, "chiusura")) == 0) {
        return CHIUSURA;
    }
    if ((strcasecmp(command, "classifica")) == 0) {
        return CLASSIFICA;
    }
    /*Fornisce un messaggio di spiegazione dei possibili comandi da impartire*/
    if ((strcasecmp(command, "help")) == 0) {
        printf("%-15s%s\n", "help", "Visualizza questo messaggio");
        printf("%-15s%s\n", "exit", "Esci dalla partita");
        if (!server) {
            printf("%-15s%s\n", "classifica", "Stampa la classifica di giocatori");
        }
        return HELP;
    }
    /*Se sono interessato ad input numerici o testuali, chiamo la funzione apposita*/
    if (inputUtente != NULL)
        return analizzaInput(inputUtente, command);
    else
        return ERRORE;
}
