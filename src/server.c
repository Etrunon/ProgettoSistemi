#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "allFifo.h"
#include "server.h"
#include "CONST.h"
#include "allFifo.h"
#include <pthread.h>
#include <stdlib.h>
#include <term.h>

int maxClients;
int maxWin;

/*Controlla che il server sia unico
 * Se vi è già un server,
 */
pthread_mutex_t mutex;
int a = 0;

void * figlioAscoltatore(void* arg) {

    return NULL;
}

int initServer(int Clients, int Win) {
    int exist = access(SERVERPATH, F_OK);
    if (exist == 0) {
        printf("Il server è gia attivo!\n");
        return -1;
    }
    maxClients = Clients;
    maxWin = Win;

    pthread_t threadID;
    pthread_create(&threadID, NULL, &figlioAscoltatore, NULL);

    pthread_join(threadID, NULL);

    return 0;
}
