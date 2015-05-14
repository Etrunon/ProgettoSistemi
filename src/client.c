#include "client.h"
#include "CONST.h"
#include "allFifo.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "parser.h"
#include "commands.h"
#include <signal.h>

int ascoltoDalServer;
int scriviAlServer;
char clientFifo [MAX_FIFONAME];
int clientID;

/*Chiude la FIFO ed eventuali altre risorse
 * rimaste aperte
 */
void cleanupClient(int sig) {
    printf("\r%30s", "Client disconnesso!");
    fflush(stdout);
    close(ascoltoDalServer);
    unlink(clientFifo);
    exit(EXIT_SUCCESS);
}

/*Gestisco la disconnesione del server*/
void serverDisconnesso(int sig) {
    printf("%30s\n", ANSI_COLOR_RED "Server disconnesso!" ANSI_COLOR_RESET);
    cleanupClient(0);
}

void * inputUtenteClient(void* arg) {
    printHelp(false);
    comando c;
    data d;

    do {
        printf("%s", "Client:");
        fflush(stdout);
        c = leggiInput(true, &d);

        switch (c) {
            case HELP:
            {
                printHelp(false);
                break;
            }
        }

    } while (c != CHIUSURA);

    cleanupClient(0);

    return NULL;
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
        printf("%40s\n", ANSI_COLOR_RED "Il server non è attivo!" ANSI_COLOR_RESET);
        return -1;
    }


    /*Apro la FIFO di ascolto dal server*/
    sprintf(clientFifo, "%s%i", CLIENTFIFO, getpid());
    ascoltoDalServer = creaFifoLettura(clientFifo);
    if (ascoltoDalServer == -1) {
        printf("%s\n", ANSI_COLOR_RED "Errore nell'apertura del client" ANSI_COLOR_RESET);
        cleanupClient(0);
        exit(EXIT_FAILURE);
    }

    /*Faccio partire l'ascoltatore di messaggi da terminale*/
    pthread_t threadID;
    pthread_create(&threadID, NULL, &inputUtenteClient, NULL);


    /*Apro la FIFO per contattare il server*/
    scriviAlServer = creaFiFoScrittura(SERVERPATH);
    if (scriviAlServer == -1) {
        printf("%s\n", ANSI_COLOR_RED "Errore nell'apertura del client" ANSI_COLOR_RESET);
        cleanupClient(0);
    }

    /*test scrittura*/
    /*
        while (true) {
            messaggio x = messaggioConstructor();
            crInvDatiRisp(x, 200);
            inviaMessaggio(x.msg, scriviAlServer);
            printf("Scritto messaggio\n");
            sleep(10);
        }
     */
    pthread_join(threadID, NULL);

    cleanupClient(0);
    return 0;
}
