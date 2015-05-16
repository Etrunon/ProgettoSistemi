#include "client.h"
#include "CONST.h"
#include "allFifo.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "parser.h"
#include "commands.h"
#include <signal.h>
#include <string.h>

int ascoltoDalServer;
int scriviAlServer;
char clientFifo [MAX_FIFONAME];
int clientID;
bool connesso = false;

/*Chiude la FIFO ed eventuali altre risorse
 * rimaste aperte
 */
void cleanupClient(int sig) {
    printf("\r%40s\n", ANSI_COLOR_CYAN "Client disattivato" ANSI_COLOR_RESET);
    fflush(stdout);
    close(ascoltoDalServer);
    unlink(clientFifo);
    exit(EXIT_SUCCESS);
}

/*Gestisco la disconnesione del server*/
void serverDisconnesso(int sig) {
    printf("%40s\n", ANSI_COLOR_RED "Server disconnesso!" ANSI_COLOR_RESET);
    cleanupClient(0);
}

void prompt() {
    printf("%s", "Client:");
    fflush(stdout);
}

void * inputUtenteClient(void* arg) {
    printHelp(false);
    comando c;
    data d;

    do {
        prompt();
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
            case HELP:
            {
                printHelp(false);
                break;
            }
            default:
                break;
        }

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
                printf("%s\n", ANSI_COLOR_RED"Il server non ha posti disponibili!"ANSI_COLOR_RESET);
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

    printf("%40s\n", ANSI_COLOR_BLUE "Client avviato" ANSI_COLOR_RESET);
    do {
        printf("%s", "Inserisci il nome:");
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

    /*Controllo se esiste un server*/
    int exist = access(SERVERPATH, F_OK);

    if (exist == -1) {
        printf("%40s\n", ANSI_COLOR_RED "Il server non è attivo!" ANSI_COLOR_RESET);
        return -1;
    }


    /*Apro la FIFO di ascolto dal server*/
    sprintf(clientFifo, "%s%i", CLIENTFIFO, getpid());
    ascoltoDalServer = creaFifoLettura(clientFifo);
    if (ascoltoDalServer == -1) {
        printf("%s\n", ANSI_COLOR_RED "Errore nell'apertura del client" ANSI_COLOR_RESET);
        cleanupClient(0);
        exit(EXIT_FAILURE);
    }

    /*Apro la FIFO per contattare il server*/
    scriviAlServer = creaFiFoScrittura(SERVERPATH);
    if (scriviAlServer == -1) {
        printf("%s\n", ANSI_COLOR_RED "Errore nell'apertura del client" ANSI_COLOR_RESET);
        cleanupClient(0);
    }

    /*Richiedo la partecipazione al gioco*/
    richiestaPartecipazione();

    ascoltaServer();

    /*Faccio partire l'ascoltatore di messaggi da terminale*/
    pthread_t threadID;
    //pthread_create(&threadID, NULL, &inputUtenteClient, NULL);
    //pthread_join(threadID, NULL);

    cleanupClient(0);
    return 0;
}
