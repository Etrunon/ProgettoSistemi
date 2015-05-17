#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "logica.h"

int maxClients;
int maxWin;
int currentClients = 0;
domanda domandaCorrente = {};
giocatore giocatoriCorrenti[10] = {};

void serverCambiaDomanda() {
    domandaCorrente.numero1 = rand() % 99;
    domandaCorrente.numero2 = rand() % 99;
}

void initLogica() {

    srand(time(NULL));
    serverCambiaDomanda();
}

int cercaSlotVuoto() {

    int i = 0;

    for (i; i < maxClients; i++) {
        if (giocatoriCorrenti[i].occupato == 0) {
            return i;
        }
    }

    return -1;
}

void serverGeneraClassifica(int* IDclients, int* punteggi) {

    int i = 0;

    for (i; i < maxClients; i++) {

        if (giocatoriCorrenti[i].occupato == 1) {
            IDclients[i] = i;
            punteggi[i] = giocatoriCorrenti[i].punteggio;
        }
    }
}

int serverAggiungiGiocatore(char* nome, int handlerFIFO) {

    int indice = cercaSlotVuoto();
    if (indice == -1)
        return -1;
    else {
        currentClients++;
        giocatoriCorrenti[indice].occupato = 1;
        giocatoriCorrenti[indice].punteggio = maxClients - currentClients;
        giocatoriCorrenti[indice].handlerFIFO = handlerFIFO;
        giocatoriCorrenti[indice].name = (char*) malloc(MAXNAME * (sizeof (char)));
        strcat(giocatoriCorrenti[indice].name, nome);

        return indice;
    }
}

bool serverAggiornaPunti(int ID, int punti) {

    if (giocatoriCorrenti[ID].occupato == 1) {
        giocatoriCorrenti[ID].punteggio = punti;

        return true;
    }
    return false;
}

int serverFIFOGiocatore(int ID) {

    if (giocatoriCorrenti[ID].occupato == 1) {
        return giocatoriCorrenti[ID].handlerFIFO;
    }
    return -1;
}

bool togliGiocatore(int ID) {

    if (giocatoriCorrenti[ID].occupato == 1) {

        free(giocatoriCorrenti[ID].name);
        giocatoriCorrenti[ID].occupato = 0;
        giocatoriCorrenti[ID].punteggio = 0;
        currentClients--;
        return true;
    }
    return false;
}

void clientAggiornaPunti(int ID, int punti) {

    giocatoriCorrenti[ID].punteggio = punti;
}

void clientAggiungiGiocatore(char* nome, int ID, int punteggio) {

    if (giocatoriCorrenti[ID].occupato == 0) {

        giocatoriCorrenti[ID].occupato = 1;
        giocatoriCorrenti[ID].punteggio = punteggio;
        giocatoriCorrenti[ID].name = (char*) malloc(MAXNAME * (sizeof (char)));
        strcat(giocatoriCorrenti[ID].name, nome);
    }

}
