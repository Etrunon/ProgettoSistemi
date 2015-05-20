#include "client.h"
#include "CONST.h"
#include "allFifo.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "riparser.h"
#include "commands.h"
#include "gui.h"
#include "logica.h"
#include "guiMessages.h"
#include <signal.h>
#include <string.h>
#include <pthread.h>

int ascoltoDalServer;
int scriviAlServer;
char clientFifo [MAX_FIFONAME];

int clientID;
char name[MAXNAME];

bool connesso = false;

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

    sprintf(msgTmp, "%s\n", "Server disconnesso!");
    aggiungiMessaggio(msgTmp, true, ANSI_COLOR_RED);
    cleanupClient(0);
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
                sprintf(msgTmp, "%s\n", "Il server non ha posti disponibili!");
                aggiungiMessaggio(msgTmp, true, ANSI_COLOR_RED);
                cleanupClient(0);
            }
                break;
            case ACCETTA_CLIENT:
            {
                //Il server ci ha accettato il login
                connesso = true;
                //Si cambia l'interfaccia
                SetGUIMode(STANDARD_CLIENT);
                //Aggiungo l'ID assegnatomi dal server alle mie variabili globali
                clientID = msg->IDOggetto;
                maxWin = msg->maxWin;
                //Mi aggiungo ai giocatori presenti nella partita
                clientAggiungiGiocatore(name, clientID, msg->punti);
                //Stampo messaggio di benvenuto
                sprintf(msgTmp, "%s%s%s\n", "Benvenuto nel gioco, ", name, "!");
                aggiungiMessaggio(msgTmp, true, ANSI_COLOR_BLUE);
            }
                break;
            case NUOVO_GIOCATORE_ENTRATO:
            {
                //E' arrivato un nuovo giocatore nella partita. Si aggiungono i suoi dati alla classifica
                clientAggiungiGiocatore(name, msg->IDOggetto, msg->punti);
                sprintf(msgTmp, "%s%s\n", msg->nomeClient, " si è unito al gioco");
                aggiungiMessaggio(msgTmp, false, NULL);

            }
                break;
            case GIOCATORE_USCITO:
            {
                //Un giocatore è uscito dalla partita e devo toglierlo dalla struttura
                togliGiocatore(msg->IDOggetto);
                sprintf(msgTmp, "%s%s\n", msg->nomeClient, " è uscito dal gioco");
                aggiungiMessaggio(msgTmp, false, NULL);
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
                    sprintf(msgTmp, "%s%s\n", tmp, " ha risposto correttamente!");
                    aggiungiMessaggio(msgTmp, false, NULL);
                }
                updateScreen();
            }
                break;
            case INVIA_DOMANDA:
            {
                //Il server mi ha inviato una nuova domanda 
                domandaCorrente.numero1 = msg->domanda1;
                domandaCorrente.numero2 = msg->domanda2;
                sprintf(msgTmp, "%s\n", "Domanda modificata");
                aggiungiMessaggio(msgTmp, false, NULL);
            }
                break;
            case VITTORIA:
            {
                //Qualcuno (anche io forse) ha vinto!
                //Controllo se ho vinto io
                if (msg->IDOggetto == clientID) {
                    sprintf(msgTmp, "%s\n", " Hai vinto!");
                    aggiungiMessaggio(msgTmp, true, ANSI_COLOR_BLUE);
                } else {
                    //Non ho vinto io :(
                    char tmp [MAXNAME];
                    getNomeGiocatore(msg->IDOggetto, tmp);
                    sprintf(msgTmp, "%s%s\n", tmp, " ha vinto!");
                    aggiungiMessaggio(msgTmp, true, ANSI_COLOR_BLUE);
                }
                //Saluto tutti e vado a casa
                cleanupClient(0);
            }
                break;
            case SERVER_SPEGNIMENTO:
            {
                //Il server si è spento. Fine dei giochi
                sprintf(msgTmp, "%s\n", "Il server si è disconnesso!");
                aggiungiMessaggio(msgTmp, true, ANSI_COLOR_BLUE);
                cleanupClient(0);
            }
                break;
            default: break;

        }

        messaggioDestructor(msg);
    }
}

int initClient() {

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
    SetGUIMode(LOGIN_CLIENT);
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
    pthread_t threadID;
    pthread_create(&threadID, NULL, &inputUtenteClient, NULL);

    /*Ascolto FIFO server*/
    ascoltaServer();

    pthread_join(threadID, NULL);


    cleanupClient(0);
    return 0;
}
