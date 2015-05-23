#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "client.h"
#include "CONST.h"
#include "allFifo.h"
#include "riparser.h"
#include "commands.h"
#include "gui.h"
#include "logica.h"
#include "guiMessages.h"
#include "messaggiASchermo.h"

int ascoltoDalServer;
int scriviAlServer;
char clientFifo [MAX_FIFONAME];

int clientID;
char name[MAXNAME];

bool connesso = false;
bool testing = false;

char msgTmp [BUFFMESSAGGIO];

void avvisaServer() {
    messaggio* logout = messaggioConstructor(clientID, LOGOUT_AL_SERVER);
    inviaMessaggio(scriviAlServer, logout);
    messaggioDestructor(logout);
}

/*Chiude la FIFO ed eventuali altre risorse
 * rimaste aperte
 */
void cleanupClient(int sig) {
    sprintf(msgTmp, "%s\n", "Client disattivato");
    SetGUIMode(EXIT_CLIENT);
    aggiungiMessaggio(msgTmp, true, ANSI_COLOR_CYAN);

    if (connesso) {
        /*Avviso che me ne sto andando*/
        avvisaServer();
    }

    close(ascoltoDalServer);
    unlink(clientFifo);
    exit(EXIT_SUCCESS);
}

/*Gestisco la disconnesione del server*/
void serverDisconnesso(int sig) {
    //cleanupClient(0);
    close(ascoltoDalServer);
    unlink(clientFifo);
    exit(EXIT_SUCCESS);
}

void * inputUtenteClient(void* arg) {

    printHelp(false);
    updateScreen();
    comando c;
    data d;

    do {
        c = leggiInput(&d);

        switch (c) {
            case RISPOSTA:
            {
                //Se l'input è una risposta E sono connesso a una partita, allora cerco di inviarla
                if (connesso) {

                    StampaRispostaInviata(d.risposta);

                    if (testing) {
                        struct timespec intervallo, intervallo2;
                        intervallo.tv_sec = 0;
                        intervallo.tv_nsec = 200000000 + 100000000 * (rand() % 30);
                        nanosleep(&intervallo, &intervallo2);
                    }

                    messaggio* msg = messaggioConstructor(clientID, INVIA_RISPOSTA);
                    msg->risposta = d.risposta;
                    inviaMessaggio(scriviAlServer, msg);
                    messaggioDestructor(msg);
                }
            }
                break;
            case LOG_EXIT:
            {
                if (connesso)
                    SetGUIMode(STANDARD_CLIENT);
                else
                    SetGUIMode(LOGIN_CLIENT);
            }
                break;
            case HELP:
            {
                printHelp(false);
            }
                break;
            case NOME:
            {
                if (!connesso) {
                    strcpy(name, d.nome);

                    messaggio* m = messaggioConstructor(clientID, RICHIESTA_PARTECIPAZIONE);
                    sprintf(m->pathFifo, "%s", clientFifo);
                    sprintf(m->nomeClient, "%s", d.nome);
                    inviaMessaggio(scriviAlServer, m);

                    messaggioDestructor(m);

                } else {
                    sprintf(msgTmp, "%s\n", "Input non valido");
                    aggiungiMessaggio(msgTmp, true, ANSI_COLOR_RED);
                }
            }
                break;
            default:
                break;
        }

        updateScreen();
    } while (c != CHIUSURA);

    cleanupClient(0);

    return NULL;
}

/*Rimane in attesa di messaggi dal server*/
void ascoltaServer() {
    while (1) {

        //Creo il messaggio vuoto da riempire con l'ascolto
        messaggio* msg = messaggioConstructor(0, 0);
        leggiMessaggio(ascoltoDalServer, msg);

        //A seconda del codice del msg eseguo l'azione correlata
        switch (msg->codiceMsg) {
            case RIFIUTA_CLIENT:
            {
                //Il server ha rifiutato il login
                StampaServerPieno();
                cleanupClient(0);
            }
                break;
            case ACCETTA_CLIENT:
            {
                //Il server ci ha accettato il login
                connesso = true;

                if (testing) {
                    pthread_t threadID;
                    pthread_create(&threadID, NULL, &inputUtenteClient, NULL);
                }

                //Si cambia l'interfaccia
                SetGUIMode(STANDARD_CLIENT);
                //Aggiungo l'ID assegnatomi dal server alle mie variabili globali
                clientID = msg->IDOggetto;
                maxWin = msg->maxWin;
                //Mi aggiungo ai giocatori presenti nella partita
                clientAggiungiGiocatore(name, clientID, msg->punti);
                //Stampo messaggio di benvenuto
                StampaBenvenutoClient(name);
            }
                break;
            case NUOVO_GIOCATORE_ENTRATO:
            {
                //E' arrivato un nuovo giocatore nella partita. Si aggiungono i suoi dati alla classifica
                strcpy(name, msg->nomeClient);
                clientAggiungiGiocatore(name, msg->IDOggetto, msg->punti);
                StampaNuovoGiocatore(msg->nomeClient);

            }
                break;
            case GIOCATORE_USCITO:
            {
                //Un giocatore è uscito dalla partita e devo toglierlo dalla struttura

                getNomeGiocatore(msg->IDOggetto, name);
                togliGiocatore(msg->IDOggetto, msg->timestring);
                StampaGiocatoreUscito(name);
            }
                break;
            case ESITO_RISPOSTA:
            {
                //Dopo aver mandato una risposta il server mi ha mandato l'esito
                clientAggiornaPunti(clientID, msg->punti);
                if (msg->corretta) {
                    sprintf(msgTmp, "%s\n", "Risposta corretta!");
                } else {
                    sprintf(msgTmp, "%s\n", "Risposta sbagliata!");
                }
                aggiungiMessaggio(msgTmp, false, NULL);
            }
                break;
            case MODIFICA_PUNTEGGIO_GIOCATORE:
            {
                //Il punteggio di qualcun altro è cambiato
                clientAggiornaPunti(msg->IDOggetto, msg->punti);
                if (msg->corretta) {
                    char tmp [MAXNAME];
                    getNomeGiocatore(msg->IDOggetto, tmp);
                    StampaEsitoRisposta(tmp, true);
                }
            }
                break;
            case INVIA_DOMANDA:
            {
                //Il server mi ha inviato una nuova domanda
                domandaCorrente.numero1 = msg->domanda1;
                domandaCorrente.numero2 = msg->domanda2;
                StampaDomandaModificata();
            }
                break;
            case VITTORIA:
            {
                //Qualcuno (anche io forse) ha vinto!
                //Controllo se ho vinto io
                if (msg->IDOggetto == clientID) {
                    StampaVittoria(NULL);
                } else {
                    //Non ho vinto io :(
                    char tmp [MAXNAME];
                    getNomeGiocatore(msg->IDOggetto, tmp);
                    StampaVittoria(tmp);
                }

                StampaPartitaTerminata();
                //Saluto tutti e vado a casa
                cleanupClient(0);
            }
                break;
            case SERVER_SPEGNIMENTO:
            {
                //Il server si è spento. Fine dei giochi
                StampaServerDisconnesso();
                cleanupClient(0);
            }
                break;
            default: break;

        }

        messaggioDestructor(msg);
    }
}

int initClient(bool testMode) {
    testing = testMode;

    /*Gestisco segnali di chiusura improvvisa dell'applicazione*/
    signal(SIGTERM, cleanupClient);
    signal(SIGINT, cleanupClient);
    signal(SIGSEGV, cleanupClient);
    signal(SIGABRT, cleanupClient);
    signal(SIGHUP, cleanupClient);

    /*Segnali di chiusura FIFO, se perdo collegamento con il server brutalmente*/
    signal(SIGPIPE, serverDisconnesso);

    /*Inizializza logica di gioco*/
    initLogica();

    /*Inizializzo GUI*/
    if (testMode) {
        SetGUIMode(TESTING_CLIENT);
    } else {
        SetGUIMode(LOGIN_CLIENT);

    }
    updateScreen();

    /*Controllo se esiste un server*/;
    int exist = access(SERVERPATH, F_OK);

    //Se non esiste un server, lo notifico e chiudo
    if (exist == -1) {
        SetGUIMode(EXIT_CLIENT);
        sprintf(msgTmp, "%s\n", "Il server non è attivo!");
        aggiungiMessaggio(msgTmp, true, ANSI_COLOR_RED);
        return -1;
    }


    /*Apro la FIFO di ascolto dal server*/
    sprintf(clientFifo, "%s%i", CLIENTFIFO, getpid());
    ascoltoDalServer = creaFifoLettura(clientFifo);
    if (ascoltoDalServer == -1) {
        sprintf(msgTmp, "%s\n", "Errore nell'apertura del client");
        aggiungiMessaggio(msgTmp, true, ANSI_COLOR_RED);
        cleanupClient(0);
        exit(EXIT_FAILURE);
    }

    /*Apro la FIFO per contattare il server*/
    scriviAlServer = creaFiFoScrittura(SERVERPATH);
    if (scriviAlServer == -1) {
        sprintf(msgTmp, "%s\n", "Errore nell'apertura del client");
        aggiungiMessaggio(msgTmp, true, ANSI_COLOR_RED);
        cleanupClient(0);
    }

    /*Faccio partire l'ascoltatore di messaggi da terminale*/
    if (!testMode) {
        pthread_t threadID;
        pthread_create(&threadID, NULL, &inputUtenteClient, NULL);
    } else {
        messaggio* partecipazione = messaggioConstructor(0, RICHIESTA_PARTECIPAZIONE);
        sprintf(partecipazione->pathFifo, "%s", clientFifo);
        char nome [MAXNAME];
        scanf("%s", nome);
        sprintf(partecipazione->nomeClient, "%s", nome);
        inviaMessaggio(scriviAlServer, partecipazione);
        messaggioDestructor(partecipazione);
    }

    /*Ascolto FIFO server*/
    ascoltaServer();

    //pthread_join(threadID, NULL);


    cleanupClient(0);
    return 0;
}
