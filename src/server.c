#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include "allFifo.h"
#include "server.h"
#include "CONST.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"

int ascoltoDaiClient;

pthread_mutex_t MutexGiocatori;

/*Chiude la FIFO ed eventuali altre risorse
 * rimaste aperte
 */
void cleanupServer(int sig) {
    printf("\r%30s", "Server disattivato");
    fflush(stdout);
    close(ascoltoDaiClient);
    unlink(SERVERPATH);
    exit(errno);
}

void * inputUtente(void* arg) {
    printHelp(true);
    comando c;
    do {
        printf("%s", "Server:");
        fflush(stdout);
        c = leggiInput(true, NULL);

        if (c == HELP)
            printHelp(true);

    } while (c != CHIUSURA);

    cleanupServer(0);

    return NULL;
}

/*Rimane in ascolto di comunicazioni dai client*/
void ascoltaClients() {
    while (1) {
        messaggio* msg = messaggioConstructor();
        leggiMessaggio(ascoltoDaiClient, msg);

        messaggioDestructor(msg);
    }
}

int initServer(int Clients, int Win) {
    /*Segnali di chiusura*/
    signal(SIGTERM, cleanupServer);
    signal(SIGINT, cleanupServer);
    signal(SIGSEGV, cleanupServer);

    /*Controllo se esiste già un server*/
    int exist = access(SERVERPATH, F_OK);
    if (exist == 0) {
        printf("%s\n", ANSI_COLOR_RED "Il server è gia attivo!" ANSI_COLOR_RESET);
        return -1;
    }
    maxClients = Clients;
    maxWin = Win;

    /* Creo la FIFO per ascoltare i client*/
    ascoltoDaiClient = creaFifoLettura(SERVERPATH);
    if (ascoltoDaiClient == -1) {
        printf("%s\n", ANSI_COLOR_RED "Errore nell'apertura del server" ANSI_COLOR_RESET);
        cleanupServer(0);
        exit(EXIT_FAILURE);
    }

    /*Messaggio di benvenuto*/
    printf("%40s\n%-40s%s%i%s\n%-40s%s%i%s\n", ANSI_COLOR_BLUE "Server avviato!" ANSI_COLOR_RESET,
            "Numero massimo di giocatori:", ANSI_COLOR_YELLOW, maxClients, ANSI_COLOR_RESET,
            "Punteggio necessario per la vittoria:", ANSI_COLOR_YELLOW, maxWin, ANSI_COLOR_RESET);

    /*Avvio thread per interazione da terminale*/
    pthread_t threadID;
    pthread_create(&threadID, NULL, &inputUtente, NULL);

    ascoltaClients();

    pthread_join(threadID, NULL);

    cleanupServer(0);

    return 0;
}
