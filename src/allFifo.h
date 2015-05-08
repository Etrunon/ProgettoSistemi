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
int creaFifoScrittura(char* path);
void chiudiFifo(char* path, bool eliminare);
bool leggiMessaggio(char* buffer, int lunghMax, int handlerFifo);
bool inviaMessaggio(char* messaggio, int handlerFifo);

#endif	/* CREAFIFO_H */

