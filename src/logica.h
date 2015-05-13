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

typedef struct domandaTag {
    int numero1;
    int numero2;
} domanda;

typedef struct classificaTag {
    int clientAttivi;
    int *listaPunti;
    char **listaNomi;
} classifica;

typedef struct partitaTag {
    domanda domanda;
    classifica *classifica;

} partita;

partita partitaConstructor();

void partitaDestructor(partita par);

domanda generaDomanda();

void processaRisposta(messaggio msg);




#endif	/* LOGICA_H */

