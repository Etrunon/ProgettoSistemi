/*
 * Progetto: Multiplayer Game
 * A.A 2014/2015
 * Carlo Mion   165878
 * Luca Bosotti 164403
 */


#ifndef GUIMESSAGES_H
#define	GUIMESSAGES_H

#include "CONST.h"
#include <stdbool.h>

typedef struct codaTag {
    int numMessaggi;
    int head;
    int tail;
    bool centrato [BUFFERMESSAGGI];
    char* messaggi [BUFFERMESSAGGI];
} coda;

extern coda codaMessaggi;

void stampaMessaggi(int numMessaggi);

void aggiungiMessaggio(char* msg, bool centrato, char* colore);

void svuotaMessaggi();
#endif	/* GUIMESSAGES_H */

