/*
 * File:   Logica.h
 * Author: etrunon
 *
 * Created on May 13, 2015, 2:42 PM
 */

#ifndef LOGICA_H
#define	LOGICA_H

#include "parser.h"

typedef struct domanda {
    int numero1;
    int numero2;
} domanda;

typedef struct giocatore {
    int handlerFIFO;
    int occupato;
    char* name;
    int punteggio;
} giocatore;

extern int maxClients;
extern int maxWin;
extern int currentClients;
extern domanda domandaCorrente;
extern giocatore giocatoriCorrenti[10];

/*INIZIALIZZAZIONE*/
void initLogica();

/*SERVER*/
/*Modifica la domanda attuale*/
void serverCambiaDomanda();

void serverGeneraClassifica(int* IDclients, int* punteggi);

/*Aggiunge un giocatore, ritorna il suo ID
 * Se non c'è spazio, ritorna -1
 */
int serverAggiungiGiocatore(char* nome, int handlerFIFO);

/* aggiunge +1 o -1 punto
 * torna true se il giocatore ha vinto*/
bool serverAggiornaPunti(int ID, int punti);

/*Ritorna handler del giocatore*/
int serverFIFOGiocatore(int ID);

/*LATO CLIENT*/
/*Aggiorna +1 o -1 punti al giocatore indicato in ID*/
void clientAggiornaPunti(int ID, int punti);

void clientAggiungiGiocatore(char* nome, int ID, int punteggio);



/*METODI COMUNI*/
bool togliGiocatore(int ID);


#endif	/* LOGICA_H */

