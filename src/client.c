#include "client.h"
#include "CONST.h"
#include "allFifo.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "parser.h"
#include "commands.h"
#include "gui.h"
#include "logica.h"
#include "guiMessages.h"
#include <signal.h>
#include <string.h>

int ascoltoDalServer;
int scriviAlServer;
char clientFifo [MAX_FIFONAME];
int clientID;
bool connesso = false;

char msgTmp [BUFFMESSAGGIO];

/*Chiude la FIFO ed eventuali altre risorse
 * rimaste aperte
 */
void cleanupClient(int sig) {
    sprintf(msgTmp, "%s\n", "Client disattivato");
    aggiungiMessaggio(msgTmp, true, ANSI_COLOR_CYAN);
    SetGUIMode(EXIT_CLIENT);
    updateScreen();
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
        c = leggiInput(true, &d);

        switch (c) {
            case RISPOSTA:
            {
                messaggio* msg = messaggioConstructor();
                msg->codiceMsg = 1;
                msg->valRisposta = d.risposta;
                inviaMessaggio(scriviAlServer, msg);
                break;
            }
            case LOG_EXIT:
            {
                if (connesso)
                    SetGUIMode(STANDARD_CLIENT);
                else
                    SetGUIMode(LOGIN_CLIENT);
                break;
            }
            case HELP:
            {
                printHelp(false);
                break;
            }
            case NOME:
            {
                if (!connesso) {
                    messaggio* m = messaggioConstructor();
                    m->codiceMsg = 2;
                    sprintf(m->pathFifo, "%s%c", clientFifo, '\0');
                    sprintf(m->nomeClient, "%s", d.nome);
                    inviaMessaggio(scriviAlServer, m);
                    messaggioDestructor(m);
                }
                break;
            }
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
        messaggio* msg = messaggioConstructor();
        leggiMessaggio(ascoltoDalServer, msg);
        switch (msg->codiceMsg) {
            case 6:
            {
                sprintf(msgTmp, "%s\n", "Il server non ha posti disponibili!");
                aggiungiMessaggio(msgTmp, true, ANSI_COLOR_RED);
                cleanupClient(0);
                break;
            }
            default: break;

        }

        messaggioDestructor(msg);
    }
}

bool richiestaPartecipazione() {
    comando c;
    data d;

    //printf("%40s\n", ANSI_COLOR_BLUE "Client avviato" ANSI_COLOR_RESET);
    do {
        printf("\r%s", "Inserisci il nome:");
        c = leggiInput(false, &d);

    } while (c != NOME);

    messaggio* m = messaggioConstructor();

    m->codiceMsg = 2;
    sprintf(m->pathFifo, "%s%c", clientFifo, '\0');
    sprintf(m->nomeClient, "%s", d.nome);

    inviaMessaggio(scriviAlServer, m);
    messaggioDestructor(m);
    return true;

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

    if (exist == -1) {
        SetGUIMode(EXIT_CLIENT);
        sprintf(msgTmp, "%s\n", "Il server non è attivo!");
        aggiungiMessaggio(msgTmp, true, ANSI_COLOR_RED);
        updateScreen();
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


    /*Richiedo la partecipazione al gioco*/
    //richiestaPartecipazione();

    /*Faccio partire l'ascoltatore di messaggi da terminale*/
    pthread_t threadID;
    pthread_create(&threadID, NULL, &inputUtenteClient, NULL);
    pthread_join(threadID, NULL);

    /*Ascolto FIFO server*/
    ascoltaServer();



    cleanupClient(0);
    return 0;
}
