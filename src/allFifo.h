/*
 * File:   creaFifo.h
 * Author: etrunon
 *
 * Created on May 7, 2015, 11:49 AM
 */

#ifndef CREAFIFO_H
#define	CREAFIFO_H

#include <stdbool.h>

int creaFifoLettura(char* path);
int apriFiFoScrittura(char* path);
int chiudiFifo(char* path, int fileDescriptor, bool eliminare);
bool leggiMessaggio(char* buffer, int lunghMax, int handlerFifo);
bool inviaMessaggio(char* messaggio, int handlerFifo);

#endif	/* CREAFIFO_H */

