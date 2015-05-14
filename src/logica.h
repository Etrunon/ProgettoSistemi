/*
 * File:   Logica.h
 * Author: etrunon
 *
 * Created on May 13, 2015, 2:42 PM
 */

#ifndef LOGICA_H
#define	LOGICA_H

#include "parser.h"

/*
 logica della partita
 * servono le strutture
 * servono le funzioni
 *
 *
 */

typedef struct domanda {
    int numero1;
    int numero2;
} domanda;

typedef struct giocatore {
    char* FIFO;
    char* name;
    int punteggio;
} giocatore;

typedef struct giocatori {
    int numeroGiocatori;
    giocatore* listaGiocatori;
} giocatori;

domanda* generaDomanda();

void processaRisposta(messaggio msg);




#endif	/* LOGICA_H */

