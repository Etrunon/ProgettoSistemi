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
#include "messaggiASchermo.h"


int ascoltoDaiClient;
//Id speciale assegnato al server
int IDServer = 0;
//Buffer temporaneo per scrivere i messaggi
char tmpMessage [BUFFMESSAGGIO];

/**
 * Funzione che dato un messaggio ne permette l'invio in "broadcast" a tutti i client
 * @param msg
 */
void broadcast(messaggio* msg) {
    int i = 0;
    for (i; i < currentClients; i++) {
        giocatore* player = giocatoriCorrenti[i];
        int fifo = player->handlerFIFO;
        inviaMessaggio(fifo, msg);
    }
}

/**
 * Routine che avvisa i client quando il server va in spegnimento
 */
void AvvisaSpegnimentoServer() {

    messaggio* msg = messaggioConstructor(IDServer, SERVER_SPEGNIMENTO);
    broadcast(msg);
    messaggioDestructor(msg);
}

/*
 * Chiude la FIFO ed eventuali altre risorse rimaste aperte.
 */
void cleanupServer(int sig) {
    if (currentClients > 0) {
        AvvisaSpegnimentoServer();
    }

    if (!testing) {
        sprintf(tmpMessage, "%s\n", "Server disattivato");
        SetGUIMode(EXIT_SERVER);
        aggiungiMessaggio(tmpMessage, true, ANSI_COLOR_CYAN);
    }
    close(ascoltoDaiClient);
    unlink(SERVERPATH);
    exit(EXIT_SUCCESS);
}

/**
 * Funzione che ascolta continuatamente l'input ricevuto dall'utente e in base ai casi dopo averlo sanitizzato
 * esegue i comandi associati.
 * @param arg
 */
void * inputUtente(void* arg) {

    printHelp(true);
    updateScreen();
    comando c;
    do {
        c = leggiInput(NULL);

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
            case STORICO:
            {
                SetGUIMode(VISUALIZZA_CLASSIFICA_SERVER);
            }
                break;
            case ERRORE:
            {
                sprintf(tmpMessage, "%s\n", "Input non valido");
                aggiungiMessaggio(tmpMessage, true, ANSI_COLOR_RED);
            }
                break;
            default: break;
        }
        updateScreen();
    } while (c != CHIUSURA);

    cleanupServer(0);
    return NULL;
}

/**
 * Funzione che dato un id di un client e un messaggio, si occupa di inviare il messaggio a tutti i client eccetto
 * quello avente come id il parametro.
 * @param IDNonAvvisare
 * @param msg
 */
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

/**
 * Funzione che invia tutti i messaggi di aggiornamento al client che si è appena aggiunto al gioco
 * @param idNuovoGiocatore
 * @param fifo
 */
void setupNuovoGiocatore(int idNuovoGiocatore, int fifo) {
    char tmp [MAXNAME];
    int id, i;
    int punti;

    /*Avviso il nuovo giocatore degli altri giocatori presenti*/
    for (i = 0; i < currentClients; i++) {
        giocatore* player = giocatoriCorrenti[i];
        id = player->IDGiocatore;
        if (id != idNuovoGiocatore) {

            messaggio* msg = messaggioConstructor(IDServer, NUOVO_GIOCATORE_ENTRATO);
            msg->punti = getPuntiGiocatore(player->IDGiocatore);
            msg->IDOggetto = id;
            getNomeGiocatore(player->IDGiocatore, tmp);
            strcpy(msg->nomeClient, tmp);

            inviaMessaggio(fifo, msg);

            messaggioDestructor(msg);
        }
    }

    /*Invio domanda corrente*/
    messaggio* domanda = messaggioConstructor(IDServer, INVIA_DOMANDA);
    domanda->domanda1 = domandaCorrente.numero1;
    domanda->domanda2 = domandaCorrente.numero2;

    inviaMessaggio(fifo, domanda);
    messaggioDestructor(domanda);
}

/**
 * Funzione che aggiunge un nuovo giocatore al gioco, allocandolo in memoria e aggiornando gli altri client della sua presenza
 * @param msg
 */
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
        messaggio* rifiutato = messaggioConstructor(IDServer, RIFIUTA_CLIENT);
        inviaMessaggio(handlerFIFO, rifiutato);
        messaggioDestructor(rifiutato);
        close(handlerFIFO);
        if (testing) {
            printf(tmpMessage, "%s%s\n", "Non ho spazio sul server per ", msg->nomeClient);
            StampaTesting(tmpMessage);
        }
        return;
    } else {

        //Stampo sul server che c'è un nuovo player
        StampaNuovoGiocatore(msg->nomeClient);

        if (testing) {
            StampaTestingGiocatore(IDGiocatore);
        }

        /*Giocatore accettato*/
        messaggio* accettato = messaggioConstructor(IDServer, ACCETTA_CLIENT);
        accettato->IDOggetto = IDGiocatore;
        accettato->punti = getPuntiGiocatore(IDGiocatore);
        accettato->maxWin = maxWin;
        inviaMessaggio(handlerFIFO, accettato);
        messaggioDestructor(accettato);

        /*Avviso tutti gli altri del nuovo arrivato*/
        messaggio* nuovoGiocatore = messaggioConstructor(IDServer, NUOVO_GIOCATORE_ENTRATO);
        strcpy(nuovoGiocatore->nomeClient, msg->nomeClient);
        nuovoGiocatore->IDOggetto = IDGiocatore;
        nuovoGiocatore->punti = getPuntiGiocatore(IDGiocatore);
        avvisaAltriClient(IDGiocatore, nuovoGiocatore);
        messaggioDestructor(nuovoGiocatore);

        /*Avviso nuovo giocatore della partita corrente*/
        setupNuovoGiocatore(IDGiocatore, handlerFIFO);
    }

}

/**
 * Routine che fa generare una nuova domanda e la invia a tutti i client in gioco
 */
void nuovaDomanda() {
    serverCambiaDomanda();

    int i;
    messaggio* domanda = messaggioConstructor(IDServer, INVIA_DOMANDA);
    domanda->domanda1 = domandaCorrente.numero1;
    domanda->domanda2 = domandaCorrente.numero2;

    broadcast(domanda);

    messaggioDestructor(domanda);
}

/**
 * Funzione che gestisce la chiusura della partita e del server in caso di vittoria di un client
 * @param ID
 * @param msg
 */
void vincitore(int ID, messaggio* msg) {

    //Mi salvo il nome del vincitore
    char nome [MAXNAME];
    getNomeGiocatore(ID, nome);

    //Stampo sul server il messaggio in cui dico che ha vinto
    StampaVittoria(nome);

    /*Print usata per avere informazioni sul giocatore in testing*/
    if (testing) {
        StampaTestingGiocatore(ID);
    }

    //Creo il messaggio di vittoria da mandare in broadcast
    messaggio* vittoria = messaggioConstructor(IDServer, VITTORIA);
    vittoria->IDOggetto = ID;
    broadcast(vittoria);
    messaggioDestructor(vittoria);

    SetGUIMode(VISUALIZZA_CLASSIFICA_SERVER);

    //Stampo a server che la partita terminata
    StampaPartitaTerminata();
    if (testing) {
        int i = 0;
        int max = currentClients;
        for (i; i < max; i++)
            togliGiocatore(giocatoriCorrenti[0]->IDGiocatore, msg->timestring);
        SetGUIMode(EXIT_CLASSIFICA);
        sprintf(tmpMessage, "%s\n", "Testing terminato!");
        aggiungiMessaggio(tmpMessage, true, ANSI_COLOR_BLUE);
        cleanupServer(0);
    }
}

/**
 * Funzione che fa controllare la risposta e ne gestisce tutte le situazioni successive,
 *  - Risposta errata
 *  - Risposta giusta, ma gioco ancora in corso
 *  - Risposta giusta e fine gioco
 * @param msg
 */
void checkRisposta(messaggio * msg) {

    //Calcolo le variabili per controllare la risposta
    int risultato = domandaCorrente.numero1 + domandaCorrente.numero2;
    int rispostaCorretta = msg->risposta;

    //Creo la stringa temporanea col nome del client
    char name [MAXNAME] = {};
    getNomeGiocatore(msg->IDMittente, name);

    //Variabile bool di gestione della routine e istanziazione messaggio di esito al client
    bool vittoria;
    messaggio* esito = messaggioConstructor(IDServer, ESITO_RISPOSTA);

    if (risultato == rispostaCorretta) {

        /*Aggiorno i suoi punti, controllo se ha vinto*/
        vittoria = serverAggiornaPunti(msg->IDMittente, 1);


        /*Ha risposto correttamente e lo stampo a video sul server*/
        StampaEsitoRisposta(name, true);

        /*Print usata per avere informazioni sul giocatore in testing*/
        if (testing) {
            StampaTestingGiocatore(msg->IDMittente);
        }

        if (vittoria) {
            /*Il client ha vinto, vado nella routine dedicata*/
            vincitore(msg->IDMittente, msg);
        } else {

            //Aggiorno il punto in più al client. Setto il booleano dell'esito a true
            esito->corretta = true;
        }
    } else {


        //Aggiorno i punti del client al meno uno e setto il bool a falso
        vittoria = serverAggiornaPunti(msg->IDMittente, -1);
        esito->corretta = false;

        /*Risposta errata, lo stampo sul server*/
        StampaEsitoRisposta(name, false);

        /*Print usata per avere informazioni sul giocatore in testing*/
        if (testing) {
            StampaTestingGiocatore(msg->IDMittente);
        }
    }

    /*Avviso il client che ha risposto correttamente o meno*/
    esito->punti = getPuntiGiocatore(msg->IDMittente);
    int fifo = serverFIFOGiocatore(msg->IDMittente);
    inviaMessaggio(fifo, esito);
    messaggioDestructor(esito);

    /*Avviso tutti gli altri client della risposta e del conseguente cambio di punteggio*/
    messaggio* risposta = messaggioConstructor(IDServer, MODIFICA_PUNTEGGIO_GIOCATORE);
    risposta->IDOggetto = msg->IDMittente;
    risposta->punti = getPuntiGiocatore(msg->IDMittente);
    avvisaAltriClient(msg->IDMittente, risposta);
    messaggioDestructor(risposta);

    if (!vittoria) {
        //Se la risposta era corretta devo generare una nuova domanda
        if (risultato == rispostaCorretta) {
            nuovaDomanda();
        }
    }
}

/**
 * Funzione che rimuove un giocatore che ha fatto logout
 * @param msg
 */
void rimuoviGiocatore(messaggio* msg) {
    char name [MAXNAME];

    /*Stampo a schermo*/
    getNomeGiocatore(msg->IDMittente, name);
    togliGiocatore(msg->IDMittente, msg->timestring);

    StampaGiocatoreUscito(name);


    /*Avviso altri client dell'uscita del giocatore*/
    messaggio* logout = messaggioConstructor(IDServer, GIOCATORE_USCITO);
    logout->IDOggetto = msg->IDMittente;
    broadcast(logout);
    messaggioDestructor(logout);

    if (currentClients == 0 && testing) {
        SetGUIMode(EXIT_CLASSIFICA);
        sprintf(tmpMessage, "%s\n", "Testing terminato!");
        aggiungiMessaggio(tmpMessage, true, ANSI_COLOR_BLUE);
        cleanupServer(0);

    }
}

/*Rimane in ascolto di comunicazioni dai client*/
void ascoltaClients() {
    while (1) {
        messaggio* msg = messaggioConstructor(0, 0);

        leggiMessaggio(ascoltoDaiClient, msg);

        switch (msg->codiceMsg) {
            case INVIA_RISPOSTA:
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

/**
 * Funzione che inizializza il server. Eseguita solo una volta all'avvio
 * @param Clients
 * @param Win
 * @return 
 */
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

    /*Ignoro la scrittura in una fifo rotta, gestendolo tramite la write() per evitare loop infiniti stile CMP*/
    signal(SIGPIPE, SIG_IGN);

    /*Avvio interfaccia grafica*/
    if (testing) {
        SetGUIMode(TESTING_SERVER);
    } else {
        calcolaLarghezzaSchermo(0);
        SetGUIMode(STANDARD_SERVER);
    }
    updateScreen();

    /*Controllo se esiste già un server*/
    int exist = access(SERVERPATH, F_OK);
    if (exist == 0) {
        sprintf(tmpMessage, "%s\n", "C'è già un altro server attivo!");
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
    if (!testing) {
        pthread_t threadID;
        pthread_create(&threadID, NULL, &inputUtente, NULL);
    }


    /*Mi metto in ascolto dei client sulla FIFO*/
    ascoltaClients();

    cleanupServer(0);

    return 0;
}
