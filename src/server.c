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
#include "logica.h"

int ascoltoDaiClient;

void AvvisaSpegnimentoServer() {
    int i = 0;
    for (i = 0; i < maxClients; i++) {
        if (giocatoriCorrenti[i].occupato) {
            messaggio* msg = messaggioConstructor();
            msg->codiceMsg = 11;
            inviaMessaggio(giocatoriCorrenti[i].handlerFIFO, msg);
            messaggioDestructor(msg);
        }
    }
}

/*Chiude la FIFO ed eventuali altre risorse
 * rimaste aperte
 */
void cleanupServer(int sig) {
    if (currentClients > 0) {
        AvvisaSpegnimentoServer();
    }
    printf("\r%40s\n", ANSI_COLOR_CYAN "Server disattivato" ANSI_COLOR_RESET);
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

void aggiungiGiocatore(messaggio* msg) {
    int handlerFIFO = creaFiFoScrittura(msg->pathFifo);

    if (handlerFIFO == -1) {
#ifdef DEBUGFIFO
        printf("%s %s\n", "FIFO errata:", msg->pathFifo);
        perror("");
#endif
        return;
    }
    int numeroGiocatore;
    numeroGiocatore = serverAggiungiGiocatore(msg->nomeClient, handlerFIFO);

    //if (numeroGiocatore == -1) {
    messaggio* rifiutato = messaggioConstructor();
    rifiutato->codiceMsg = 6;
    inviaMessaggio(handlerFIFO, rifiutato);
    messaggioDestructor(rifiutato);
    close(handlerFIFO);
    //}

    //TODO GESTIONE IMPOSSIBILE AGGIUNGERE
}

/*Rimane in ascolto di comunicazioni dai client*/
void ascoltaClients() {
    while (1) {
        messaggio* msg = messaggioConstructor();

        leggiMessaggio(ascoltoDaiClient, msg);
        switch (msg->codiceMsg) {
            case 1:
            {

                break;

            }
            case 2:
            {
                aggiungiGiocatore(msg);
                break;
            }
        }
        messaggioDestructor(msg);
    }
}

int initServer(int Clients, int Win) {
    /*Segnali di chiusura*/
    signal(SIGTERM, cleanupServer);
    signal(SIGINT, cleanupServer);
    signal(SIGSEGV, cleanupServer);
    signal(SIGABRT, cleanupServer);
    signal(SIGHUP, cleanupServer);

    /*Ignoro la scrittura in una fifo rotta, gestendolo tramite la write()*/
    signal(SIGPIPE, SIG_IGN);

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
    printf("%40s\n%-40s%s%i%s\n%-40s%s%i%s\n", ANSI_COLOR_BLUE "Server avviato" ANSI_COLOR_RESET,
            "Numero massimo di giocatori:", ANSI_COLOR_YELLOW, maxClients, ANSI_COLOR_RESET,
            "Punteggio necessario per la vittoria:", ANSI_COLOR_YELLOW, maxWin, ANSI_COLOR_RESET);

    /*Inizializza le strutture dati relative al gioco*/
    initLogica();

    /*Avvio thread per interazione da terminale*/
    pthread_t threadID;
    pthread_create(&threadID, NULL, &inputUtente, NULL);

    /*Mi metto in ascolto dei client sulla FIFO*/
    ascoltaClients();

    pthread_join(threadID, NULL);

    cleanupServer(0);

    return 0;
}
