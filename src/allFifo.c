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
 * Funzione che crea una fifo da LETTURA al path specificato, aprendola in READ and WRITE
 * @param path stringa con il path bersaglio
 * @return int hand, handler della fifo
 */
void gestisciErrore(char* path) {
    if (errno == EEXIST) {
        printf("Errore, il file della fifo %s esiste\n", path);
    }
}

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
 * @param path stringa con il path beraglio
 * @return hand, handler della pipe
 * @exception -1 se la FIFO non esiste o nessuno la ha aperta in lettura
 */
int creaFiFoScrittura(char* path) {

    int hand = 0;
    hand = open(path, O_WRONLY | O_NONBLOCK);

    return hand;
}

/**
 * Funzione che chiude una fifo e se delete è true ne cerca di eliminare il file
 * @param handler, handler della fifo da chiudere
 * @param delete, settare vero se si vuole eliminare la fifo
 */
int chiudiFifo(char* path, int fileDescriptor, bool eliminare) {
    int errore = 0;

    errore = close(fileDescriptor);
    if (errore == -1) {
        return errore;
    }

    errore = unlink(path);

    if (errore = -1) {
        return errore;
    }
    return errore;
}

/**
 * Funzione che legge un messaggio dall'handler specificato, lo riconverte da stringa a messaggio logico e lo ritorna
 * @param handlerFifo
 * @param msg
 * @return 
 */
bool leggiMessaggio(int handlerFifo, messaggio *msg) {
    int letti;

    letti = read(handlerFifo, msg->msg, MSG_SIZE + 1);

    traduciComm(msg);
#ifdef DEBUGFIFO
    testStampaMessaggio(msg, "Ricezione");
#endif
    return true;
}

/**
 * Funzione che prende in input un messaggio logico, lo converte in stringa tramite la libreria riparser 
 * e lo invia all'handler specificato
 * @param handlerFifo
 * @param msg
 * @return 
 */
bool inviaMessaggio(int handlerFifo, messaggio *msg) {

    creaMessaggio(msg);
    int written = write(handlerFifo, msg->msg, MSG_SIZE + 1);

#ifdef DEBUGFIFO
    testStampaMessaggio(msg, "Invio");
#endif

    memset(msg->msg, 0, MSG_SIZE);

    /*L'altro lato della FIFO si è disconnesso*/
    if (written == -1)
        return false;
    else
        return true;
}