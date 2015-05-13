/*
 * File:   commands.h
 * Author: mion00
 *
 * Created on May 13, 2015, 7:28 PM
 */

#ifndef COMMANDS_H
#define	COMMANDS_H

#include "CONST.h"

/*Tipo di comando che l'utente ha dato in input*/
typedef enum comando {
    CHIUSURA,
    CLASSIFICA,
    HELP,
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

/** Interpreta l'input dell'utente, spiegando i comandi possibili
 * e gestendo i casi di input errato
 * @param server
 * Se sto leggendo input dal server o dal client
 * @param inputUtente
 * Mettere NULL se non si è interessati alla risposta alla domanda
 * o al nome dell'utente
 * @return
 * Il tipo di comando impartito dall'utente
 */
comando leggiInput(bool server, data* inputUtente);

#endif	/* COMMANDS_H */

