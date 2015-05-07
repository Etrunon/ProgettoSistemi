#include "creaFifo.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

/**
 * Funzione che crea una fifo da LETTURA al path specificato, aprendola in READ and WRITE
 * @param path stringa con il path bersaglio
 * @return int hand, handler della fifo
 */
void gestisciErrore() {

}

int creaFifoLettura(char* path) {

    int hand = 0, errCheck = 0;

    /*
     S_IWUSR write permission owner
     S_IRUSR read permission owner
     S_IRGRP read permission group
     S_IROTH read permission other
     */
    errCheck = mkfifo(path, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);

    if (errCheck == -1) {
        gestisciErrore();
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
        gestisciErrore();
    }

    //Questa chiamata blocca il processo nel caso in cui non ci sia nessuno a leggere dalla fifo
    hand = open(path, O_WRONLY | O_NONBLOCK);

    return hand;
}


