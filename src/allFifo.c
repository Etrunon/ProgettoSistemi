#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>

#include "allFifo.h"

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
        gestisciErrore(path);
    }

    hand = open(path, O_RDWR);

    /*
     TODO gestione errore, stampare errore e ritornare -1 a livello esterno
     */
    return hand;
}

/**
 * Funzione che crea una fifo da SCRITTURA al path specificato, aprendola WRITE only
 * Nel caso in cui nessuno stia leggendo La funzione rileva uno stato d'errore e lo comunica tramite un valore di
 *      hand di -1
 * @param path stringa con il path beraglio
 * @return hand, handler della pipe
 */
int creaFifoScrittura(char* path) {

    int hand = 0, errCheck = 0;

    /*
     S_IWUSR write permission owner
     S_IRUSR read permission owner
     S_IRGRP read permission group
     S_IROTH read permission other
     */
    errCheck = mkfifo(path, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);

    if (errCheck == -1) {
        gestisciErrore(path);
    }

    //Questa chiamata blocca il processo nel caso in cui non ci sia nessuno a leggere dalla fifo
    hand = open(path, O_WRONLY | O_NONBLOCK);

    return hand;
}

/**
 * Funzione che chiude una fifo e se delete è true ne cerca di eliminare il file
 * @param handler, handler della fifo da chiudere
 * @param delete, settare vero se si vuole eliminare la fifo
 */
void chiudiFifo(char* path, bool eliminare) {
    int errore = 0;
    errore = unlink(path);

    if (errore = -1) {
        gestisciErrore(path);
    }

    /*
     TODO gestire eliminazione
     */
}

bool leggiMessaggio(char* buffer, int lunghMax, int handlerFifo) {

    read(handlerFifo, buffer, 99);
    //perror("Dentro leggi messaggio: ");

    return true;
}

bool inviaMessaggio(char* messaggio, int handlerFifo) {

    write(handlerFifo, messaggio, sizeof (messaggio));

    //perror("Dentro invia messaggio: ");
    return true;
}