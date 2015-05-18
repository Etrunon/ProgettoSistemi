/*
 * File:   guiMessages.h
 * Author: mion00
 *
 * Created on May 16, 2015, 10:19 PM
 */

#ifndef GUIMESSAGES_H
#define	GUIMESSAGES_H

#include "CONST.h"
#include <stdbool.h>

typedef struct codaTag {
    int numMessaggi;
    int head;
    int tail;
    char* messaggi [BUFFERMESSAGGI];
} coda;

extern coda codaMessaggi;

void stampaMessaggi(int numMessaggi);

void aggiungiMessaggio(char* msg, bool centrato, char* colore);

void svuotaMessaggi();
#endif	/* GUIMESSAGES_H */

