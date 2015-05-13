#include "client.h"
#include "CONST.h"
#include "allFifo.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int fifo;
char clientFifo [MAX_FIFONAME];
int clientID;

/*Chiude la FIFO ed eventuali altre risorse
 * rimaste aperte
 */
void cleanupClient(int sig) {
    //printf("\n%30s\n", "Disattivazione del client!");
    close(fifo);
    unlink(clientFifo);
    exit(EXIT_SUCCESS);
}

void * ascoltaServer(void* arg) {
    while (1) {
        leggiMessaggio();
    }
    return NULL;
}

int initClient() {
    /*Controllo se esiste un server*/
    int exist = access(SERVERPATH, F_OK);
    /*
        if (exist == -1) {
            printf("%s\n", "Il server non è attivo!");
            return -1;
        }
     */

    /*Apro la FIFO di ascolto dal server*/
    sprintf(clientFifo, "%s%i", CLIENTFIFO, getpid());
    fifo = creaFifoLettura(clientFifo);
    if (fifo == -1) {
        printf("%s\n", "Errore nell'apertura del client");
        cleanupClient(0);
        exit(EXIT_FAILURE);
    }

    cleanupClient(0);

    return 0;
}
