/*
 * Progetto: Multiplayer Game
 * A.A 2014/2015
 * Carlo Mion   165878
 * Luca Bosotti 164403
 */

#ifndef COMMANDS_H
#define	COMMANDS_H

#include <stdbool.h>
#include "CONST.h"

/*Tipo di comando che l'utente ha dato in input*/
typedef enum comando {
    CHIUSURA,
    STORICO,
    HELP,
    LOG_SHOW,
    LOG_EXIT,
    RISPOSTA,
    NOME,
    ERRORE
} comando;

/** Struttura atta a contenere l'input dell'utente, una volta riconosciuto
 */
typedef struct data {
    int risposta;
    char nome [MAXNAME];
} data;

comando leggiInput(data* inputUtente);

/*Fornisce un messaggio di spiegazione dei possibili comandi da impartire*/
void printHelp(bool server);
#endif	/* COMMANDS_H */

