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
    printf("\n%30s\n", "Client disconnesso!");
    close(ascoltoDalServer);
    unlink(clientFifo);
    exit(EXIT_SUCCESS);
}

/*Gestisco la disconnesione del server*/
void serverDisconnesso(int sig) {
    printf("%30s\n", "Server disconnesso!");
    cleanupClient(0);
}

/*Thread che rimane in attesa di messaggi dal server*/
void * ascoltaServer(void* arg) {
    while (1) {
        messaggio* m = (messaggio*) malloc(sizeof (messaggio));
        leggiMessaggio(ascoltoDalServer, m);
    }
    return NULL;
}

int initClient() {
    /*Gestisco segnali di chiusura improvvisa dell'applicazione*/
    signal(SIGTERM, cleanupClient);
    signal(SIGINT, cleanupClient);

    /*Segnali di chiusura FIFO, se perdo collegamento con il server brutalmente*/
    signal(SIGPIPE, serverDisconnesso);

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
    }

    /*test scrittura*/
    while (true) {
        messaggio x = messaggioConstructor();
        crInvDatiRisp(x, 200);
        inviaMessaggio(x.msg, scriviAlServer);
        printf("Scritto messaggio\n");
        sleep(10);
    }
    pthread_join(threadID, NULL);

    cleanupClient(0);
    return 0;
}
