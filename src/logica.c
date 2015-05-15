#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "logica.h"

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

int serverAggiungiGiocatore(char* nome, char* FIFO) {

    int indice = cercaSlotVuoto();
    if (indice == -1)
        return -1;
    else {
        giocatoriCorrenti[indice].occupato = 1;

        giocatoriCorrenti[indice].name = (char*) malloc(MAXNAME * (sizeof (char)));
        strcat(giocatoriCorrenti[indice].name, nome);

        giocatoriCorrenti[indice].FIFO = malloc(MAX_FIFONAME * (sizeof (char)));
        strcat(giocatoriCorrenti[indice].FIFO, FIFO);
    }
    return;
}

bool serverAggiornaPunti(int ID, int punti) {

    if (giocatoriCorrenti[ID].occupato == 1) {
        giocatoriCorrenti[ID].punteggio = punti;

        return true;
    }
    return false;
}

char* serverFIFOGiocatore(int ID) {

    if (giocatoriCorrenti[ID].occupato == 1) {
        return giocatoriCorrenti[ID].FIFO;
    }
    return NULL;
}

bool togliGiocatore(int ID) {

    if (giocatoriCorrenti[ID].occupato == 1) {

        memset(giocatoriCorrenti[ID].FIFO, 0, MAX_FIFONAME);
        memset(giocatoriCorrenti[ID].name, 0, ID);
        giocatoriCorrenti[ID].occupato = 0;
        giocatoriCorrenti[ID].punteggio = 0;

        return true;
    }
    return false;
}

void clientAggiornaPunti(int ID, int punti) {

    giocatoriCorrenti[ID].punteggio = punti;
}

void clientNuovoGiocatore(char* nome, int ID, int punti) {

    if (giocatoriCorrenti[ID].occupato == 0) {

        giocatoriCorrenti[ID].occupato = 1;

        giocatoriCorrenti[ID].name = (char*) malloc(ID * (sizeof (char)));
        strcat(giocatoriCorrenti[ID].name, nome);
    }

}
