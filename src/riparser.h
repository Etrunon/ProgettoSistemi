/*
 * File:   riparser.h
 * Author: etrunon
 *
 * Created on May 15, 2015, 12:53 PM
 */

#ifndef RIPARSER_H
#define	RIPARSER_H

#include <stdbool.h>

typedef enum tipoMessaggio {
    INVIA_RISPOSTA,
    RICHIESTA_PARTECIPAZIONE,
    LOGOUT_AL_SERVER,
    ACCETTA_CLIENT,
    RIFIUTA_CLIENT,
    NUOVO_GIOCATORE_ENTRATO,
    GIOCATORE_USCITO,
    MODIFICA_PUNTEGGIO_GIOCATORE,
    ESITO_RISPOSTA,
    INVIA_DOMANDA,
    SERVER_SPEGNIMENTO,
    VITTORIA
} tipoMessaggio;

typedef struct messaggioTag {
    int PIDMittente;
    tipoMessaggio codiceMsg;
    char* timestring;
    char* msg;
    char* pathFifo;
    char* nomeClient;
    int risposta;
    int IDMittente;
    int punti;
    int domanda1;
    int domanda2;
    bool corretta;
    int IDOggetto;
    int maxWin;

} messaggio;

messaggio* messaggioConstructor(int IDMittente, tipoMessaggio tipoMsg);

void messaggioDestructor(messaggio *x);

bool creaMessaggio(messaggio *msg);

void traduciComm(messaggio *msg);


#endif	/* RIPARSER_H */

