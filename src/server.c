#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include "allFifo.h"
#include "server.h"
#include "CONST.h"
#include "allFifo.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"

int maxClients;
int maxWin;
int ascoltoDaiClient;

pthread_mutex_t mutex;
int a = 0;

/*Chiude la FIFO ed eventuali altre risorse
 * rimaste aperte
 */
void cleanupServer(int sig) {
    printf("\n%30s\n", "Server disattivato");
    close(ascoltoDaiClient);
    unlink(SERVERPATH);
    exit(errno);
}

void * inputUtente(void* arg) {
    comando c;
    data* d = (data*) malloc(sizeof (data));

    do {
        c = leggiInput(true, d);
        printf("%s\n", d->nome);

    } while (c != CHIUSURA);

    return NULL;
}

int initServer(int Clients, int Win) {
    /*Segnali di chiusura*/
    signal(SIGTERM, cleanupServer);
    signal(SIGINT, cleanupServer);
    signal(SIGSEGV, cleanupServer);

    /*Controllo se esiste già un server*/
    int exist = access(SERVERPATH, F_OK);
    if (exist == 0) {
        printf("Il server è gia attivo!\n");
        return -1;
    }
    maxClients = Clients;
    maxWin = Win;

    /* Creo la FIFO per ascoltare i client*/
    ascoltoDaiClient = creaFifoLettura(SERVERPATH);
    if (ascoltoDaiClient == -1) {
        printf("%s\n", "Errore nell'apertura del server");
        cleanupServer(0);
        exit(EXIT_FAILURE);
    }

    /*Messaggio di benvenuto*/
    printf("%30s\n%-40s%i\n%-40s%i\n", "Server avviato!",
            "Numero massimo di giocatori:", maxClients,
            "Punteggio necessario per la vittoria:", maxWin);

    /*Avvio thread per interazione da terminale*/
    pthread_t threadID;
    pthread_create(&threadID, NULL, &inputUtente, NULL);

    //Test lettura
    /*
        while (1) {
            //printf("%s\n", "In attesa di messaggi!");

            //messaggio msg = messaggioConstructor();
            //leggiMessaggio(ascoltoDaiClient, &msg);
            //printf("Messaggio: %s", msg.msg);
            sleep(2);
            printf("\r%-30s", "Un messaggio!");
            fflush(stdout);
            sleep(2);
            printf("\r%-30s", "Un altro messaggio!");
            fflush(stdout);
            //leggiComm(msg, msg.msg);
        }
     */


    pthread_join(threadID, NULL);
    cleanupServer(0);

    return 0;
}
