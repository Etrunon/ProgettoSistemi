/*
 * Progetto: Multiplayer Game
 * A.A 2014/2015
 * Carlo Mion   165878
 * Luca Bosotti 164403
 */


#ifndef CREAFIFO_H
#define	CREAFIFO_H

#include <stdbool.h>

#include "parser.h"
#include "CONST.h"

int creaFifoLettura(char* path);
int creaFiFoScrittura(char* path);
int chiudiFifo(char* path, int fileDescriptor, bool eliminare);

bool leggiMessaggio(int handlerFifo, messaggio *msg);
bool inviaMessaggio(int handlerFifo, messaggio *msg);

#endif	/* CREAFIFO_H */

