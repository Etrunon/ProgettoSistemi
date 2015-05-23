
/*
 * File:   Logica.h
 * Author: etrunon
 *
 * Created on May 13, 2015, 2:42 PM
 */

#ifndef LOGICA_H
#define	LOGICA_H

#include "parser.h"
#include "CONST.h"

typedef struct domanda {
    int numero1;
    int numero2;
} domanda;

typedef struct giocatoreTag {
    int IDGiocatore;
    int handlerFIFO;
    char* name;
    int punteggio;
} giocatore;

typedef struct saveStatTag {
    giocatore *g;
    char *time;
} saveStat;

//Costante col limite massimo di giocatori
extern int maxClients;
//Costante col limite massimo di punti alla vittoria
extern int maxWin;
//Variabile con il numero di client presenti al momento nel gioco
extern int currentClients;
//Inizializzazione della domanda in corso
extern domanda domandaCorrente;
//Inizializzazione a null dell'array dei giocatori correnti
extern giocatore* giocatoriCorrenti[11];
//Variabile contenente il prossimo ID da assegnare a un client

extern int indiceStorico;

saveStat* storico[200];

/*INIZIALIZZAZIONE*/
/**
 * Funzione che inizializza la parte logica del SERVER
 */
void initLogica();

/*SERVER*/

/**
 * Funzione che cambia la domanda corrente con una nuova
 */
void serverCambiaDomanda();

/**
 * Funzione che riempie gli array passati per parametro con i punteggi dei giocatori presenti.
 * Assume che la classifica sia già correttamente ordinata.
 * @param IDclients array con gli id
 * @param punteggi  array con i punti
 */
void serverGeneraClassifica(int* IDclients, int* punteggi);

/**
 * Aggiunge un giocatore, ritorna il suo ID
 * Se non c'è spazio, ritorna -1
 * @param nome stringa col nome del client da aggiungere
 * @param handlerFIFO handler del nuovo giocatore
 * @return ID del client aggiunto
 */
int serverAggiungiGiocatore(char* nome, int handlerFIFO);

/**
 * Funzione che aggiunge o toglie UN punto solo. Lancia automaticamente un ordinamento della classifica
 * Ritorna true se il giocatore in questione ha vinto
 * @param ID
 * @param punti
 * @return
 */
bool serverAggiornaPunti(int ID, int punti);

/**
 * Funzione che dato un ID ritorna l'handler della sua FIFO
 * @param ID bersaglio
 * @return handler della FIFO
 */
int serverFIFOGiocatore(int ID);

/*LATO CLIENT*/

/**
 * Funzione che TODO
 * @param ID del giocatore bersaglio
 * @param punti da modificare
 */
void clientAggiornaPunti(int ID, int punti);

/**
 * Funzione che aggiunge il giocatore specificato alla lista di quelli presenti nella partita insieme al client
 * Vengono settati il nome, l'ID e i punti (non come delta ma come set bruto)
 * @param nome
 * @param ID
 * @param punteggio
 */
void clientAggiungiGiocatore(char* nome, int ID, int punteggio);



/*METODI COMUNI*/

/**
 * Metodo che toglie il giocatore specificato dalla lista di quelli presenti a seguito per esempio di un
 * logout. Riordina la lista di giocatori presenti.
 * @param ID
 */
void togliGiocatore(int ID, char* timestring);

/**
 * Getter del nome del giocatore. Riempie una stringa sufficientemente lunga passata per puntatore.
 * @param ID
 * @param riempi
 */
void getNomeGiocatore(int ID, char* riempi);

/**
 * Getter dei punti del giocatore con ID.
 * @param ID
 * @return punti
 */
int getPuntiGiocatore(int ID);

/**
 * FUnzione che riempie la stringa dta per parametro con le informazioni del giocatore
 * @param g
 * @param riempi
 * @return 
 */
void* StampaGiocatore(giocatore *g, char* riempi);

/**
 * Funzione che dato un id ritorna l'indirizzo del giocatore nell'array, oppure -1 se non lo trova
 * @param ID
 * @return 
 */
int cercaGiocatore(int ID);
#endif	/* LOGICA_H */

