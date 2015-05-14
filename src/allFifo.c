/*
 * costruttore messggio                 x
 * metodi invio ricevi
 * creazione messaggio metodo unico
 * refactoring con funzioni coerenti
 */


#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "allFifo.h"
#include "CONST.h"
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
int apriFiFoScrittura(char* path) {

    int hand = 0;

    /*
     S_IWUSR write permission owner
     S_IRUSR read permission owner
     S_IRGRP read permission group
     S_IROTH read permission other
     */
    //errCheck = mkfifo(path, O_CREAT | S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);

    /*
        if (errCheck == -1) {
            perror("mkfifo:");
            return -1;
        }
     */

    //Questa chiamata blocca il processo nel caso in cui non ci sia nessuno a leggere dalla fifo
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

bool leggiMessaggio(int handlerFifo, messaggio *msg) {
    int letti;

    letti = read(handlerFifo, msg->msg, MSG_SIZE + 1);

    //printf("Letti: %i\n", letti);
    //perror("Dentro leggi messaggio: ");

    return true;
}

bool inviaMessaggio(int handlerFifo, messaggio *msg) {

    msg->msg = (char*) malloc(MSG_SIZE * (sizeof (char)));
    creaMessaggio(msg);
    printf("Ecco il messaggio finito \t %s \n", msg->msg);
    //write(handlerFifo, messaggio, sizeof (messaggio));
    //perror("Dentro invia messaggio: ");

    return true;
}