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
    for (i = 0; i < currentClients; i++) {
        int fifo = (giocatoriCorrenti[i])->handlerFIFO;
        messaggio* msg = messaggioConstructor();
        msg->domandaNum1 = 11;
        inviaMessaggio(fifo, msg);
        messaggioDestructor(msg);
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

void avvisaAltriClient(int IDNonAvvisare, messaggio* msg) {
    int i = 0;
    for (i; i < currentClients; i++) {
        giocatore* player = giocatoriCorrenti[i];
        if (player->IDGiocatore != IDNonAvvisare) {
            int fifo = player->handlerFIFO;
            inviaMessaggio(fifo, msg);
        }
    }
}

void aggiungiGiocatore(messaggio * msg) {
    int handlerFIFO = creaFiFoScrittura(msg->pathFifo);

    /*Errore apertura FIFO client*/
    if (handlerFIFO == -1) {
#ifdef DEBUGFIFO
        printf("%s %s\n", "FIFO errata:", msg->pathFifo);
        perror("");
#endif
        return;
    }
    int IDGiocatore;
    IDGiocatore = serverAggiungiGiocatore(msg->nomeClient, handlerFIFO);

    /*Non c'è spazio nel server*/
    if (IDGiocatore == -1) {
        messaggio* rifiutato = messaggioConstructor();
        rifiutato->codiceMsg = 5;
        inviaMessaggio(handlerFIFO, rifiutato);
        messaggioDestructor(rifiutato);
        close(handlerFIFO);
        return;
    } else {
        messaggio* accettato = messaggioConstructor();
        accettato->codiceMsg = ACCETTA_CLIENT;
        accettato->IDOggetto = IDGiocatore;
        inviaMessaggio(handlerFIFO, accettato);
        messaggioDestructor(accettato);

        messaggio* nuovoGiocatore = messaggioConstructor();
        nuovoGiocatore->codiceMsg = 6;
        strcpy(nuovoGiocatore->nomeClient, msg->nomeClient);
        nuovoGiocatore->IDOggetto = IDGiocatore;
        nuovoGiocatore->clientPunti = getPuntiGiocatore(IDGiocatore);
        /*Avviso altri giocatori del nuovo arrivato*/
        avvisaAltriClient(IDGiocatore, nuovoGiocatore);
        messaggioDestructor(nuovoGiocatore);
    }

}

void checkRisposta(messaggio * msg) {
    int risultato = domandaCorrente.numero1 + domandaCorrente.numero2;
    int risposta = domandaCorrente.numero1 + domandaCorrente.numero2;

    bool vittoria;
    if (risultato == risposta) {
        char name [MAXNAME];
        getNomeGiocatore(msg->clientID, name);
        sprintf(tmpMessage, "%s%s\n", name, " ha risposto correttamente!");
        aggiungiMessaggio(tmpMessage, false, NULL);
        vittoria = serverAggiornaPunti(msg->clientID, 1);
        updateScreen();
        if (vittoria) {
            messaggio* vittoria = messaggioConstructor();

        }
        /*Avviso client che ha risposto correttamente*/
        messaggio* corretto = messaggioConstructor();
        corretto->codiceMsg = ESITO_RISPOSTA;
        corretto->corretta = true;
        int fifo = serverFIFOGiocatore(msg->clientID);
        inviaMessaggio(fifo, corretto);
        messaggioDestructor(corretto);
        /*TODO CHECK VITTORIA*/
        /*Avviso altri client della risposta*/
        messaggio* risposta = messaggioConstructor();
        risposta->codiceMsg = MODIFICA_PUNTEGGIO_GIOCATORE;
        risposta->IDOggetto = msg->clientID;
        risposta->clientPunti = -1;
        avvisaAltriClient(msg->clientID, risposta);
        messaggioDestructor(risposta);

    } else /*Risposta errata*/ {
        char name [MAXNAME];
        getNomeGiocatore(msg->clientID, name);
        sprintf(tmpMessage, "%s%s\n", name, " ha sbagliato risposta!");
        aggiungiMessaggio(tmpMessage, false, NULL);
        updateScreen();

        /*Avviso client della risposta sbagliata*/
        messaggio* sbagliato = messaggioConstructor();
        sbagliato->codiceMsg = ESITO_RISPOSTA;
        sbagliato->corretta = false;
        int fifo = serverFIFOGiocatore(msg->clientID);
        inviaMessaggio(fifo, sbagliato);
        messaggioDestructor(sbagliato);

        /*Avviso altri client del punto sbagliato*/
        messaggio* risposta = messaggioConstructor();
        risposta->codiceMsg = MODIFICA_PUNTEGGIO_GIOCATORE;
        risposta->IDOggetto = msg->clientID;
        risposta->clientPunti = -1;
        avvisaAltriClient(msg->clientID, risposta);
        messaggioDestructor(risposta);
    }
}

void rimuoviGiocatore(messaggio* msg) {
    char name [MAXNAME];
    getNomeGiocatore(msg->clientID, name);
    sprintf(tmpMessage, "%s%s\n", name, " si è disconnesso");
    aggiungiMessaggio(tmpMessage, false, NULL);
    updateScreen();
    messaggio* logout = messaggioConstructor();
    logout->codiceMsg = GIOCATORE_USCITO;
    logout->IDOggetto = msg->clientID;
    avvisaAltriClient(msg->clientID, logout);
    messaggioDestructor(logout);
}

/*Rimane in ascolto di comunicazioni dai client*/
void ascoltaClients() {
    while (1) {
        messaggio* msg = messaggioConstructor();

        leggiMessaggio(ascoltoDaiClient, msg);
        switch (msg->codiceMsg) {
            case RISPOSTA:
            {
                checkRisposta(msg);
                break;
            }
            case RICHIESTA_PARTECIPAZIONE:
            {
                aggiungiGiocatore(msg);
                break;
            }
            case LOGOUT_AL_SERVER:
            {
                rimuoviGiocatore(msg);
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
