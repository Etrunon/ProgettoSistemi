#include "client.h"
#include "CONST.h"
#include "allFifo.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "parser.h"
#include <signal.h>

int ascoltoDalServer;
int scriviAlServer;
char clientFifo [MAX_FIFONAME];
int clientID;

/*Chiude la FIFO ed eventuali altre risorse
 * rimaste aperte
 */
void cleanupClient(int sig) {
    close(ascoltoDalServer);
    unlink(clientFifo);
    exit(EXIT_SUCCESS);
}

void * ascoltaServer(void* arg) {
    while (1) {
        messaggio* m = (messaggio*) malloc(sizeof (messaggio));
        leggiMessaggio(ascoltoDalServer, m);
    }
    return NULL;
}

int initClient() {
    /*Segnali di chiusura*/
    signal(SIGTERM, cleanupClient);
    signal(SIGINT, cleanupClient);

    /*Controllo se esiste un server*/
    int exist = access(SERVERPATH, F_OK);

    if (exist == -1) {
        printf("%s\n", "Il server non è attivo!");
        return -1;
    }


    /*Apro la FIFO di ascolto dal server*/
    sprintf(clientFifo, "%s%i", CLIENTFIFO, getpid());
    ascoltoDalServer = creaFifoLettura(clientFifo);
    if (ascoltoDalServer == -1) {
        printf("%s\n", "Errore nell'apertura del client");
        cleanupClient(0);
        exit(EXIT_FAILURE);
    }

    /*Faccio partire l'ascoltatore di messaggi dal server*/
    pthread_t threadID;
    pthread_create(&threadID, NULL, &ascoltaServer, NULL);

    /*Apro la FIFO per contattare il server*/
    scriviAlServer = apriFiFoScrittura(SERVERPATH);
    if (scriviAlServer == -1) {
        printf("%s\n", "Errore nell'apertura del client");
        cleanupClient(0);
        exit(EXIT_FAILURE);
    }

    /*test scrittura*/
    messaggio *x = messaggioConstructor();
    inviaMessaggio(scriviAlServer, x);
    printf("Scritto messaggio\n");

    pthread_join(threadID, NULL);

    cleanupClient(0);
    return 0;
}
