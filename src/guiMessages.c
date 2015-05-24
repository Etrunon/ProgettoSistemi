/*
 * Progetto: Multiplayer Game
 * A.A 2014/2015
 * Carlo Mion   165878
 * Luca Bosotti 164403
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include "guiMessages.h"
#include "gui.h"

/*Mutex che protegge l'aggiunta dei messaggi alla coda*/
pthread_mutex_t aggiungiMessaggioMutex = PTHREAD_MUTEX_INITIALIZER;

/*Inizializza la coda*/
coda codaMessaggi = {
    0,
    0,
    0,
    {},
    {}
};

/*Funzione che permette di calcolare l'indice da cui stampare i messaggi,
 considerando l'oridne cronologico di arrivo e la posizione dell'ultimo messaggio
 nella coda circolare*/
int returnFromTail(int tail, int steps) {
    int result = tail - steps;
    if (tail - steps < 0) {
        result = tail - steps + BUFFERMESSAGGI;
    }
    return result;
}

/*Stampa un numero di messaggi pari al parametro passato,
 *mostrandoli in ordine fino all'ultimo ricveuto */
void stampaMessaggi(int numMessaggi) {
    if (numMessaggi > 0) {
        int i = codaMessaggi.head;
        int daStampare = codaMessaggi.numMessaggi;

        if (numMessaggi < codaMessaggi.numMessaggi) {
            i = returnFromTail(codaMessaggi.tail, numMessaggi);
            daStampare = numMessaggi;
        }

        while (daStampare > 0) {
            char tmpBuffer1 [BUFFMESSAGGIO] = {};

            /*Controlla se il messaggio è da centrare, e se lo è, calcola il padding appropriato*/
            if (codaMessaggi.centrato[i]) {
                sprintf(tmpBuffer1, "%*s", larghezzaSchermo / 2 + (int) strlen(codaMessaggi.messaggi[i]) / 2, codaMessaggi.messaggi[i]);
            } else {
                sprintf(tmpBuffer1, "%s", codaMessaggi.messaggi[i]);
            }
            printf("\r%s", tmpBuffer1);
            i = (i + 1) % BUFFERMESSAGGI;
            daStampare--;
        }
    }

};

/*Aggiunge un messaggio alla coda, utilizzado il mutex per proteggerla*/
void aggiungiMessaggio(char* msg, bool centrato, char* colore) {
    pthread_mutex_lock(&aggiungiMessaggioMutex);

    char tmpBuffer [BUFFMESSAGGIO] = {};

    char* tmp = codaMessaggi.messaggi[codaMessaggi.tail];
    if (tmp != NULL) {
        /*Sto sovrascrivendo un messaggio perché ho riempito la coda */
        free(tmp);
        codaMessaggi.head = (codaMessaggi.head + 1) % BUFFERMESSAGGI;
        codaMessaggi.centrato[codaMessaggi.tail] = false;
    } else {
        codaMessaggi.numMessaggi++;
    }
    codaMessaggi.messaggi[codaMessaggi.tail] = (char*) malloc(sizeof (char)*BUFFMESSAGGIO);

    if (colore != NULL) {
        /*Applica il colore al messaggio, se indicato*/
        char tmpBuffer1[BUFFMESSAGGIO] = {};

        strcpy(tmpBuffer1, colore);

        char tmp = msg[strlen(msg) - 1];
        msg[strlen(msg) - 1] = '\0';

        strcat(tmpBuffer1, msg);
        strcat(tmpBuffer1, ANSI_COLOR_RESET);

        sprintf(tmpBuffer, "%s%c", tmpBuffer1, tmp);

    } else {
        strcpy(tmpBuffer, msg);
    }

    if (centrato) {
        codaMessaggi.centrato[codaMessaggi.tail] = true;
    }

    strcpy(codaMessaggi.messaggi[codaMessaggi.tail], tmpBuffer);
    codaMessaggi.tail = (codaMessaggi.tail + 1) % BUFFERMESSAGGI;

    pthread_mutex_unlock(&aggiungiMessaggioMutex);
    updateScreen();
};

/*Utility per svuotare la coda dei messaggi*/
void svuotaMessaggi() {
    int i = codaMessaggi.head;
    int daCancellare = codaMessaggi.numMessaggi;

    while (daCancellare > 0) {
        if (codaMessaggi.messaggi[i] != NULL) {
            free(codaMessaggi.messaggi[i]);
            codaMessaggi.messaggi[i] = NULL;
        }
        i = (i + 1) % BUFFERMESSAGGI;
        daCancellare--;
    }

    codaMessaggi.head = 0;
    codaMessaggi.tail = 0;
    codaMessaggi.numMessaggi = 0;
}