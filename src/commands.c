/*
 * Progetto: Multiplayer Game
 * A.A 2014/2015
 * Carlo Mion   165878
 * Luca Bosotti 164403
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include "commands.h"

#include "CONST.h"
#include "logica.h"
#include "guiMessages.h"
#include "gui.h"

/*
 * Cerca di analizzare l'input come risposta numerico o stringa
 * L'utente può aver inserito un numero come risposta
 * L'utente può aver inserito il suo nome per loggarsi al server
 */
comando analizzaInput(data* output, char* input) {
    char* carattereErrore = NULL;
    int numero;
    numero = strtol(input, &carattereErrore, 10);
    if (strcmp(carattereErrore, "\0") == 0) {
        //se ho un numero, controllo se è nel range ammissibile per essere inviato al server
        if (numero > 999999 || numero < -99999) {
            char msg [BUFFMESSAGGIO];
            sprintf(msg, "%s\n", "Risposta non ammissibile");
            aggiungiMessaggio(msg, true, ANSI_COLOR_RED);
            return ERRORE;
        }
        output->risposta = numero;
        return RISPOSTA;
    } else {
        /*L'utente ha inserito una stringa, che interpreto come il suo nome*/
        strcpy(output->nome, input);
        return NOME;
    }
    return ERRORE;
}

/** Interpreta l'input dell'utente gestendo i casi di input errato
 * @param inputUtente
 * struct udata per ritornare il nome o la risposta, se riconosciuta nell'input
 * @return
 * Il tipo di comando impartito dall'utente
 */
comando leggiInput(data* inputUtente) {
    char* commands = NULL;
    size_t size;
    char command [MAXCOMMAND] = {};
    char msgAschermo [BUFFMESSAGGIO];

    /*Leggo l'intera linea da terminale*/
    fflush(stdin);
    int read = getline(&commands, &size, stdin);

    if (read > 1) {
        /*Se ho letto qualcosa, chiudo la stringa letta da terminale con \0*/
        commands[read - 1] = '\0';
    }

    /*Controllo di non avere stringhe troppo lunghe in input*/
    if (strlen(commands) > MAXCOMMAND) {
        sprintf(msgAschermo, "%s\n", "Input non valido");
        aggiungiMessaggio(msgAschermo, true, ANSI_COLOR_RED);
        return ERRORE;
    }

    /*Considero solo la prima parola inviata nell'intera linea*/
    sscanf(commands, "%s", command);
    free(commands);

    if (strlen(command) == 0) {
        /*L'utente ha premuto Return senza inserire niente*/
        return ERRORE;
    };

    /*Controllo se l'input è un comando da eseguire, e nel caso ritorno il comando relativo come valore di enumeratore*/

    if ((strcasecmp(command, "exit")) == 0) {
        return CHIUSURA;
    }
    if ((strcasecmp(command, "storico")) == 0) {
        return STORICO;
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

    /*Se sono interessato ad input numerici o testuali, chiamo la funzione per decodificarli*/
    if (inputUtente != NULL)
        return analizzaInput(inputUtente, command);
    else
        return ERRORE;
}

/*Stampa il messaggio di help con i comandi disponibili, diversi se si è client o server*/
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

    if (server) {
        sprintf(msg, "%-15s%s\n", "storico", "Visualizza i giocatori che hanno partecipato al gioco");
        aggiungiMessaggio(msg, false, ANSI_COLOR_GREEN);
    }

}