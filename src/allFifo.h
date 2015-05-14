/*
 * File:   creaFifo.h
 * Author: etrunon
 *
 * Created on May 7, 2015, 11:49 AM
 */

#ifndef CREAFIFO_H
#define	CREAFIFO_H

#include <stdbool.h>

#include "parser.h"
#include "CONST.h"


/*Crea una FIFO in lettura, tornando -1 se l'operazione non va a buon fine*/
int creaFifoLettura(char* path);
int apriFiFoScrittura(char* path);
int chiudiFifo(char* path, int fileDescriptor, bool eliminare);
bool leggiMessaggio(int handlerFifo, messaggio *msg);
bool inviaMessaggio(int handlerFifo, messaggio *msg);

#endif	/* CREAFIFO_H */

