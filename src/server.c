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

void broadcast(messaggio* msg) {
    int i = 0;
    for (i; i < currentClients; i++) {
        giocatore* player = giocatoriCorrenti[i];
        int fifo = player->handlerFIFO;
        inviaMessaggio(fifo, msg);
    }
}

void AvvisaSpegnimentoServer() {
    messaggio* msg = messaggioConstructor();
    msg->codiceMsg = SERVER_SPEGNIMENTO;
    broadcast(msg);

    messaggioDestructor(msg);

}

/*Chiude la FIFO ed eventuali altre risorse
 * rimaste aperte
 */
void cleanupServer(int sig) {
    if (currentClients > 0) {
        AvvisaSpegnimentoServer();
    }
    sprintf(tmpMessage, "%s\n", "Server disattivato");
    SetGUIMode(EXIT_SERVER);
    aggiungiMessaggio(tmpMessage, true, ANSI_COLOR_CYAN);

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
            }
                break;
            case HELP:
            {
                printHelp(true);
            }
                break;
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

void setupNuovoGiocatore(int idNuovoGiocatore, int fifo) {
    char tmp [MAXNAME];
    int id, i;
    int punti;

    /*Avviso nuovo giocatore degli altri giocatori presenti*/
    for (i = 0; i < currentClients; i++) {
        giocatore* player = giocatoriCorrenti[i];
        id = player->IDGiocatore;
        if (id != idNuovoGiocatore) {
            punti = getPuntiGiocatore(player->IDGiocatore);
            getNomeGiocatore(player->IDGiocatore, tmp);

            messaggio* msg = messaggioConstructor();
            msg->codiceMsg = NUOVO_GIOCATORE_ENTRATO;
            msg->IDOggetto = id;
            strcpy(msg->nomeClient, tmp);
            msg->punti = punti;

            inviaMessaggio(fifo, msg);

            messaggioDestructor(msg);
        }
    }

    /*Invio domanda corrente*/
    messaggio* domanda = messaggioConstructor();
    domanda->codiceMsg = INVIA_DOMANDA;
    domanda->domanda1 = domandaCorrente.numero1;
    domanda->domanda2 = domandaCorrente.numero2;

    inviaMessaggio(fifo, domanda);

    messaggioDestructor(domanda);
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
        rifiutato->codiceMsg = RIFIUTA_CLIENT;
        inviaMessaggio(handlerFIFO, rifiutato);
        messaggioDestructor(rifiutato);
        close(handlerFIFO);
        return;
    } else {
        /*Giocatore accettato*/
        messaggio* accettato = messaggioConstructor();
        accettato->codiceMsg = ACCETTA_CLIENT;
        accettato->IDOggetto = IDGiocatore;
        inviaMessaggio(handlerFIFO, accettato);
        messaggioDestructor(accettato);

        /*Avviso tutti del nuovo arrivato*/
        messaggio* nuovoGiocatore = messaggioConstructor();
        nuovoGiocatore->codiceMsg = NUOVO_GIOCATORE_ENTRATO;
        strcpy(nuovoGiocatore->nomeClient, msg->nomeClient);
        nuovoGiocatore->IDOggetto = IDGiocatore;
        nuovoGiocatore->punti = getPuntiGiocatore(IDGiocatore);
        broadcast(nuovoGiocatore);
        messaggioDestructor(nuovoGiocatore);

        /*Avviso nuovo giocatore della partita corrente*/
        setupNuovoGiocatore(IDGiocatore, handlerFIFO);
    }

}

void nuovaDomanda() {
    serverCambiaDomanda();

    int i;
    messaggio* domanda = messaggioConstructor();
    domanda->codiceMsg = INVIA_DOMANDA;
    domanda->domanda1 = domandaCorrente.numero1;
    domanda->domanda2 = domandaCorrente.numero2;

    broadcast(domanda);

    messaggioDestructor(domanda);
}

void vincitore(int ID) {
    char nome [MAXNAME];
    getNomeGiocatore(ID, nome);

    sprintf(tmpMessage, "%s%s\n", nome, " ha vinto!");
    aggiungiMessaggio(tmpMessage, true, ANSI_COLOR_YELLOW);

    messaggio* vittoria = messaggioConstructor();
    vittoria->codiceMsg = VITTORIA;
    vittoria->IDOggetto = ID;

    broadcast(vittoria);
    messaggioDestructor(vittoria);

    sprintf(tmpMessage, "%s\n", "Partita terminata!");
    aggiungiMessaggio(tmpMessage, true, ANSI_COLOR_YELLOW);
    cleanupServer(0);
}

void checkRisposta(messaggio * msg) {
    int risultato = domandaCorrente.numero1 + domandaCorrente.numero2;
    int rispostaCorretta = msg->risposta;

    char name [MAXNAME];
    getNomeGiocatore(msg->IDMittente, name);

    bool vittoria;
    if (risultato == rispostaCorretta) {
        /*Ha risposto correttamente*/

        sprintf(tmpMessage, "%s%s\n", name, " ha risposto correttamente!");
        aggiungiMessaggio(tmpMessage, false, NULL);

        /*Aggiorno i suoi punti, controllo se ha vinto*/
        vittoria = serverAggiornaPunti(msg->IDMittente, 1);
        if (vittoria) {
            /*Client ha vinto*/
            vincitore(msg->IDMittente);
        } else {

            serverAggiornaPunti(msg->IDMittente, getPuntiGiocatore(msg->IDMittente) + 1);

            /*Avviso client che ha risposto correttamente*/
            messaggio* corretto = messaggioConstructor();
            corretto->codiceMsg = ESITO_RISPOSTA;
            corretto->corretta = true;
            int fifo = serverFIFOGiocatore(msg->IDMittente);
            inviaMessaggio(fifo, corretto);
            messaggioDestructor(corretto);

            /*Cambio domanda*/
            nuovaDomanda();
        }
    } else {
        /*Risposta errata*/
        sprintf(tmpMessage, "%s%s\n", name, " ha sbagliato risposta!");
        aggiungiMessaggio(tmpMessage, false, NULL);

        serverAggiornaPunti(msg->IDMittente, getPuntiGiocatore(msg->IDMittente) - 1);

        /*Avviso client della risposta sbagliata*/
        messaggio* sbagliato = messaggioConstructor();
        sbagliato->codiceMsg = ESITO_RISPOSTA;
        sbagliato->corretta = false;
        int fifo = serverFIFOGiocatore(msg->IDMittente);
        inviaMessaggio(fifo, sbagliato);
        messaggioDestructor(sbagliato);

    }

    /*Avviso tutti i client della risposta*/
    messaggio* risposta = messaggioConstructor();
    risposta->codiceMsg = MODIFICA_PUNTEGGIO_GIOCATORE;
    risposta->IDOggetto = msg->IDMittente;
    risposta->punti = getPuntiGiocatore(msg->IDMittente);
    broadcast(risposta);
    messaggioDestructor(risposta);
}

void rimuoviGiocatore(messaggio* msg) {
    char name [MAXNAME];
    togliGiocatore(msg->IDMittente);

    /*Stampo a schermo*/
    getNomeGiocatore(msg->IDMittente, name);
    sprintf(tmpMessage, "%s%s\n", name, " si è disconnesso");
    aggiungiMessaggio(tmpMessage, false, NULL);


    /*Avviso altri client dell'uscita del giocatore*/
    messaggio* logout = messaggioConstructor();
    logout->codiceMsg = GIOCATORE_USCITO;
    logout->IDOggetto = msg->IDMittente;
    broadcast(logout);
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
            }
                break;
            case RICHIESTA_PARTECIPAZIONE:
            {
                aggiungiGiocatore(msg);
            }
                break;
            case LOGOUT_AL_SERVER:
            {
                rimuoviGiocatore(msg);
            }
                break;
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
        SetGUIMode(EXIT_SERVER);
        aggiungiMessaggio(tmpMessage, true, ANSI_COLOR_RED);
        return -1;
    }

    /* Creo la FIFO per ascoltare i client*/
    ascoltoDaiClient = creaFifoLettura(SERVERPATH);
    if (ascoltoDaiClient == -1) {
        sprintf(tmpMessage, "%s\n", "Errore nell'apertura del server");
        SetGUIMode(EXIT_SERVER);
        aggiungiMessaggio(tmpMessage, true, ANSI_COLOR_RED);
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
