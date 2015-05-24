/*
 * Progetto: Multiplayer Game
 * A.A 2014/2015
 * Carlo Mion   165878
 * Luca Bosotti 164403
 */


#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "allFifo.h"
#include "parser.h"

/**
 * Funzione che crea una fifo da LETTURA al path specificato, aprendola in modalità READ and WRITE
 * (È necessario utilizzare questa modalità, altrimenti utilizzando solo la modalità READ i messaggi rimangono nella FIFO nonstante avvenga correttamente la read().
 * @param path stringa con il path bersaglio
 * @return int hand, handler della fifo
 */
int creaFifoLettura(char* path) {

    int hand = 0, errCheck = 0;

    /*
     S_IWUSR write permission owner
     S_IRUSR read, bool delete permission owner
     S_IRGRP read permission group
     S_IROTH read permission other
     */
    errCheck = mkfifo(path, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);

    if (errCheck == -1) {
        return -1;
    }

    hand = open(path, O_RDWR);

    return hand;
}

/**
 * Funzione che apre la FIFO al path specificato, aprendola WRITE only
 * Nel caso in cui nessuno stia leggendo o la FIFO non esista la funzione rileva uno stato d'errore e lo comunica tramite un valore di
 *      hand di -1
 * @param path stringa con il path bersaglio
 * @return hand, handler della FIFO
 * @exception -1 se la FIFO non esiste o nessuno la ha aperta in lettura
 */
int creaFiFoScrittura(char* path) {

    int hand = 0;
    hand = open(path, O_WRONLY | O_NONBLOCK);

    return hand;
}

/**
 * Funzione che legge un messaggio dall'handler specificato e lo converte da stringa a messaggio logico
 * @param handlerFifo
 * @param msg
 * @return
 */
bool leggiMessaggio(int handlerFifo, messaggio *msg) {
    int letti;

    letti = read(handlerFifo, msg->msg, MSG_SIZE + 1);

    traduciComm(msg);
#ifdef DEBUGFIFO
    /*Print di debug*/
    testStampaMessaggio(msg, "Ricezione");
#endif
    return true;
}

/**
 * Funzione che prende in input un messaggio logico, lo converte in stringa tramite la libreria riparser
 * e lo invia tramite l'handler specificato
 * @param handlerFifo
 * @param msg
 * @return
 */
bool inviaMessaggio(int handlerFifo, messaggio *msg) {

    creaMessaggio(msg);
    int written = write(handlerFifo, msg->msg, MSG_SIZE + 1);

#ifdef DEBUGFIFO
    /*Print di debug*/
    testStampaMessaggio(msg, "Invio");
#endif

    memset(msg->msg, 0, MSG_SIZE);

    /*L'altro lato della FIFO si è disconnesso*/
    if (written == -1)
        return false;
    else
        return true;
}