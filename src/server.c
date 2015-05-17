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
#include "gui.h"
#include "guiMessages.h"

int ascoltoDaiClient;

char tmpMessage [BUFFMESSAGGIO];

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
    sprintf(tmpMessage, "%s\n", "Server disattivato");
    aggiungiMessaggio(tmpMessage, true, ANSI_COLOR_CYAN);
    SetGUIMode(EXIT_SERVER);
    updateScreen();

    close(ascoltoDaiClient);
    unlink(SERVERPATH);
    exit(errno);
}

void * inputUtente(void* arg) {
    printHelp(true);
    updateScreen();
    comando c;
    do {
        c = leggiInput(true, NULL);

        switch (c) {
            case LOG_EXIT:
            {
                SetGUIMode(STANDARD_SERVER);
                break;
            }
            case HELP:
            {
                printHelp(true);
                break;
            }
        }
        updateScreen();
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
    maxClients = Clients;
    maxWin = Win;

    /*Inizializza le strutture dati relative al gioco*/
    initLogica();

    /*Segnali di chiusura*/
    signal(SIGSEGV, cleanupServer);
    signal(SIGABRT, cleanupServer);
    /*Chiusura terminale*/
    signal(SIGHUP, cleanupServer);
    signal(SIGTERM, cleanupServer);
    /*Ctrl-C*/
    signal(SIGINT, cleanupServer);
    /*Ctrl-Z*/
    signal(SIGTSTP, cleanupServer);
    signal(SIGSTOP, cleanupServer);
    /*Ctrl-\*/
    signal(SIGQUIT, cleanupServer);

    /*Ignoro la scrittura in una fifo rotta, gestendolo tramite la write()*/
    signal(SIGPIPE, SIG_IGN);

    /*Avvio interfaccia grafica*/
    SetGUIMode(STANDARD_SERVER);
    updateScreen();

    /*Controllo se esiste già un server*/
    int exist = access(SERVERPATH, F_OK);
    if (exist == 0) {
        sprintf(tmpMessage, "%s\n", "Il server è gia attivo!");
        aggiungiMessaggio(tmpMessage, true, ANSI_COLOR_RED);
        SetGUIMode(EXIT_SERVER);
        updateScreen();
        return -1;
    }

    /* Creo la FIFO per ascoltare i client*/
    ascoltoDaiClient = creaFifoLettura(SERVERPATH);
    if (ascoltoDaiClient == -1) {
        sprintf(tmpMessage, "%s\n", "Errore nell'apertura del server");
        aggiungiMessaggio(tmpMessage, true, ANSI_COLOR_RED);
        SetGUIMode(EXIT_SERVER);
        updateScreen();
        cleanupServer(0);
        exit(EXIT_FAILURE);
    }

    /*Avvio thread per interazione da terminale*/
    pthread_t threadID;
    pthread_create(&threadID, NULL, &inputUtente, NULL);


    /*Mi metto in ascolto dei client sulla FIFO*/
    ascoltaClients();

    //testGUI(true);

    //pthread_join(threadID, NULL);

    cleanupServer(0);

    return 0;
}
