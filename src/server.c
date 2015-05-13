#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "allFifo.h"
#include "server.h"
#include "CONST.h"
#include "allFifo.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


int maxClients;
int maxWin;
int ascoltoDaiClient;

pthread_mutex_t mutex;
int a = 0;

/*Chiude la FIFO ed eventuali altre risorse
 * rimaste aperte
 */
void cleanupServer(int sig) {
    printf("\n%30s\n", "Disattivazione del server!");
    close(ascoltoDaiClient);
    unlink(SERVERPATH);
    exit(EXIT_SUCCESS);
}

void * inputUtente(void* arg) {
    char* commands = (char*) malloc(sizeof (char) * MAXCOMMAND);
    size_t size = sizeof (commands);
    char command [MAXCOMMAND];

    while (strcmp(command, "exit")) {

        printf("%s", "Server:");
        int read = getline(&commands, &size, stdin);
        commands[read - 1] = '\0';
        sscanf(commands, "%s", command);
    }
    return NULL;
}

int initServer(int Clients, int Win) {
    /*Segnali di chiusura*/
    signal(SIGTERM, cleanupServer);
    signal(SIGINT, cleanupServer);
    signal(SIGSEGV, cleanupServer);

    /*Controllo se esiste già un server*/
    int exist = access(SERVERPATH, F_OK);
    if (exist == 0) {
        printf("Il server è gia attivo!\n");
        return -1;
    }
    maxClients = Clients;
    maxWin = Win;

    /* Creo la FIFO per ascoltare i client*/
    ascoltoDaiClient = creaFifoLettura(SERVERPATH);
    if (ascoltoDaiClient == -1) {
        printf("%s\n", "Errore nell'apertura del server");
        cleanupServer(0);
        exit(EXIT_FAILURE);
    }

    /*Messaggio di benvenuto*/
    printf("%30s\n%-40s%i\n%-40s%i\n", "Server avviato!",
            "Numero massimo di giocatori:", maxClients,
            "Punteggio necessario per la vittoria:", maxWin);

    /*Avvio thread per interazione da terminale*/
    pthread_t threadID;
    pthread_create(&threadID, NULL, &inputUtente, NULL);

    /*Test lettura*/
    while (1) {
        printf("%s\n", "In attesa di messaggi!");
        //Luca
        messaggio *msg = messaggioConstructor();

        leggiMessaggio(ascoltoDaiClient, msg);
        printf("Messaggio: %s\n", msg->msg);
    }


    pthread_join(threadID, NULL);
    cleanupServer(0);

    return 0;
}
