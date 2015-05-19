#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "logica.h"

int maxClients;
int maxWin;
int currentClients = 0;
domanda domandaCorrente = {};
giocatore* giocatoriCorrenti[10] = {};
int prossimoID = 1;

giocatore* giocatoreConstructor() {

    giocatore *g = (giocatore*) malloc(1 * (sizeof (giocatore)));
    g->name = (char*) malloc(MAXNAME * (sizeof (char)));
    return g;
}

void giocatoreDestructor(giocatore *g) {
    free(g->name);
    free(g);
}

char* testStampaGiocatore(giocatore *g) {


    printf("Ecco il giocatore %s\n", g->name);
    printf("ID: %i, Handler: %i, Punti: %i\n", g->IDGiocatore, g->handlerFIFO, g->punteggio);
    printf("\n");
}

void testStampaClassifica(char* str) {

    printf("%s\n", str);
    printf("Ecco la classifica\n");
    printf("Sono presenti %i giocatori su un massimo di %i\n\n", currentClients, maxClients);

    int i = 0;
    for (i; i < currentClients; i++) {
        testStampaGiocatore(giocatoriCorrenti[i]);
    }
    printf("\tFine classifica\n\n");
}

void serverCambiaDomanda() {
    domandaCorrente.numero1 = rand() % 99;
    domandaCorrente.numero2 = rand() % 99;
}

void initLogica() {

    srand(time(NULL));
    serverCambiaDomanda();
}

/**
 * Funzione che swappa i giocatori nel caso in cui i punteggi siano maggiori l'uno dell'altro.
 * @param indicePartenza
 * @return 
 */
void swap(int index) {

    giocatore *tmp;
    bool mosso;

    if (currentClients == 1)
        return;

    do {

        mosso = false;
        //Blocco che checka gli estremi e swappa nel caso.

        if (giocatoriCorrenti[index] != NULL && giocatoriCorrenti[index + 1] != NULL) {
            if (giocatoriCorrenti[index]->punteggio < giocatoriCorrenti[index + 1]->punteggio && index < currentClients) {
                tmp = giocatoriCorrenti[index];
                giocatoriCorrenti[index] = giocatoriCorrenti[index + 1];
                giocatoriCorrenti[index + 1] = tmp;
                mosso = true;
                index++;
            }
        }

        if (giocatoriCorrenti[index] != NULL && giocatoriCorrenti[index - 1] != NULL) {
            if (index != 0 && giocatoriCorrenti[index - 1]->punteggio < giocatoriCorrenti[index]->punteggio) {
                tmp = giocatoriCorrenti[index];
                giocatoriCorrenti[index] = giocatoriCorrenti[index - 1];
                giocatoriCorrenti[index - 1] = tmp;
                mosso = true;
                index--;
            }
        }

    } while (mosso == true);
}

/**
 * Funzione che riempie gli array passati per parametro con i punteggi dei giocatori presenti.
 * Assume che la classifica sia già correttamente ordinata.
 * @param IDclients
 * @param punteggi
 */
void serverGeneraClassifica(int* IDclients, int* punteggi) {

    int i = 0;
    for (i; i < currentClients; i++) {
        if (giocatoriCorrenti[i] != NULL) {
            IDclients[i] = giocatoriCorrenti[i]->IDGiocatore;
            punteggi[i] = giocatoriCorrenti[i]->punteggio;
        }
    }
}

/*Aggiunge un giocatore, ritorna il suo ID
 * Se non c'è spazio, ritorna -1
 */
int serverAggiungiGiocatore(char* nome, int handlerFIFO) {

    //Creo un giocatore e lo riempio coi dati in input
    giocatore* g = giocatoreConstructor();
    g->handlerFIFO = handlerFIFO;
    strcpy(g->name, nome);
    g->IDGiocatore = prossimoID;
    prossimoID++;
    g->punteggio = maxClients - currentClients - 1;

    //Assegno il giocatore alla struttura dati
    giocatoriCorrenti[currentClients] = g;
    swap(currentClients);
    currentClients++;


    //Ritorno l'id assegnato al client
    return prossimoID - 1;
}

/**
 * Funzione che dato un ID di un giocatore lo cerca dentro la struttura dati.
 * Ritorna -1 se non lo trova
 * @param ID
 * @return 
 */
int cercaGiocatore(int ID) {

    int i = 0;
    for (i; i < currentClients; i++) {
        if (giocatoriCorrenti[i]->IDGiocatore == ID)
            return i;
    }

    return -1;
}

/**
 * Funzione che aggiunge o toglie UN punto solo. Lancia automaticamente un ordinamento della classifica
 * Ritorna true se il giocatore in questione ha vinto
 * @param ID
 * @param punti
 * @return 
 */
bool serverAggiornaPunti(int ID, int punti) {

    int index = cercaGiocatore(ID);

    if (giocatoriCorrenti[index]->punteggio + punti <= -1) {
        giocatoriCorrenti[index]->punteggio = 0;

    } else
        giocatoriCorrenti[index]->punteggio += punti;

    swap(index);

    if (giocatoriCorrenti[index]->punteggio >= maxWin)
        return true;
    else
        return true;
}

/*Ritorna handler del giocatore*/
int serverFIFOGiocatore(int ID) {

    int i = cercaGiocatore(ID);
    return giocatoriCorrenti[i]->handlerFIFO;
}

/*LATO CLIENT*/

/*Aggiorna +1 o -1 punti al giocatore indicato in ID*/
void clientAggiornaPunti(int ID, int punti) {

    int i = cercaGiocatore(ID);
    giocatoriCorrenti[i]->punteggio = punti;
}

void clientAggiungiGiocatore(char* nome, int ID, int punteggio) {

    //Creo un giocatore e lo riempio coi dati in input
    giocatore* g = giocatoreConstructor();
    strcpy(g->name, nome);
    g->IDGiocatore = prossimoID;
    prossimoID++;
    g->punteggio = maxClients - currentClients;

    //Assegno il giocatore alla struttura dati
    giocatoriCorrenti[currentClients] = g;
    swap(currentClients);
    currentClients++;

}

/*METODI COMUNI*/
bool togliGiocatore(int ID) {

    int i = cercaGiocatore(ID);
    giocatoriCorrenti[i]->punteggio = -1;
    swap(i);

    currentClients--;

    giocatore *p = giocatoriCorrenti[currentClients];
    giocatoriCorrenti[currentClients] = NULL;
    giocatoreDestructor(p);

}

void getNomeGiocatore(int ID, char* riempi) {

    int i = cercaGiocatore(ID);
    strcpy(riempi, giocatoriCorrenti[i]->name);
}

int getPuntiGiocatore(int ID) {

    int i = cercaGiocatore(ID);
    return giocatoriCorrenti[i]->punteggio;
}