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
    int occupato;
    char* FIFO;
    char* name;
    int punteggio;
} giocatore;

static int maxClients;
static int maxWin;
static int currentClients;
static domanda domandaCorrente;
static giocatore giocatoriCorrenti[10];

/*INIZIALIZZAZIONE*/
void initLogica();

/*SERVER*/
/*Modifica la domanda attuale*/
void serverCambiaDomanda();

void serverGeneraClassifica(int* IDclients, int* punteggi);

/*Aggiunge un giocatore, ritorna il suo ID
 * Se non c'è spazio, ritorna -1
 */
int serverAggiungiGiocatore(char* nome, char* FIFO);

/*torna true se il giocatore ha vinto*/
bool serverAggiornaPunti(int ID, int punti);

char* serverFIFOGiocatore(int ID);

bool togliGiocatore(int ID);

/*LATO CLIENT*/
/*Setta un utente all'interno del client ai punti attuali*/
void clientAggiornaPunti(int ID, int punti);

void clientNuovoGiocatore(char* nome, int ID, int punti);

#endif	/* LOGICA_H */

