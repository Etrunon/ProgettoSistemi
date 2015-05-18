/*
 * File:   creaFifo.h
 * Author: etrunon
 *
 * Created on May 7, 2015, 11:49 AM
 */

#ifndef CREAFIFO_H
#define	CREAFIFO_H

#include <stdbool.h>

#include "riparser.h"
#include "CONST.h"


/*Crea una FIFO in lettura, tornando -1 se l'operazione non va a buon fine*/
int creaFifoLettura(char* path);
int creaFiFoScrittura(char* path);
int chiudiFifo(char* path, int fileDescriptor, bool eliminare);
bool leggiMessaggio(int handlerFifo, messaggio *msg);

/*Invia messaggio nella FIFO, ritorna false se l'altro lato in lettura è disconnesso*/
bool inviaMessaggio(int handlerFifo, messaggio *msg);

#endif	/* CREAFIFO_H */

