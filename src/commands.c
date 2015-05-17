#include "CONST.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "commands.h"
#include "logica.h"
#include "guiMessages.h"
#include "gui.h"

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
            char msg [BUFFMESSAGGIO];
            sprintf(msg, "%s\n", "Risposta non ammissibile");
            aggiungiMessaggio(msg, true, ANSI_COLOR_RED);
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
        char msg [BUFFMESSAGGIO];
        sprintf(msg, "%s\n", "Input non valido");
        aggiungiMessaggio(msg, true, ANSI_COLOR_RED);
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
    if ((strcasecmp(command, "log")) == 0) {
        SetGUIMode(LOG);
        return LOG_SHOW;
    }
    if ((strcasecmp(command, "q")) == 0) {
        return LOG_EXIT;
    }
    /*Se sono interessato ad input numerici o testuali, chiamo la funzione apposita*/
    if (inputUtente != NULL)
        return analizzaInput(inputUtente, command);
    else
        return ERRORE;
}

void printHelp(bool server) {
    char msg [BUFFMESSAGGIO];
    sprintf(msg, "%s\n", "Comandi disponibili");
    aggiungiMessaggio(msg, true, ANSI_COLOR_GREEN);

    sprintf(msg, "%-15s%s\n", "help", "Visualizza questo messaggio");
    aggiungiMessaggio(msg, false, ANSI_COLOR_GREEN);

    sprintf(msg, "%-15s%s\n", "log", "Mostra lo storico dei messaggi");
    aggiungiMessaggio(msg, false, ANSI_COLOR_GREEN);

    sprintf(msg, "%-15s%s\n", "exit", "Esci dalla partita");
    aggiungiMessaggio(msg, false, ANSI_COLOR_GREEN);

    if (!server) {
        sprintf(msg, "%-15s%s\n", "classifica", "Stampa la classifica di giocatori");
        aggiungiMessaggio(msg, false, ANSI_COLOR_GREEN);
    }
}