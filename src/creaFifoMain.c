#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/stat.h>

#include <stdio.h>

#include <time.h>
#include <errno.h>

#include "allFifo.h"

/*
 * Todo:
 */
void handler(int signo) {
    printf("Errore FIFO rotta");
}

int main(int argc, char** argv) {

    //handler dei SIGNAL
    signal(SIGPIPE, handler);
    char* path = "/tmp/testfifo";

    //Test LibreriaFIFO
    int hand = apriFiFoScrittura(path);

    if (hand == -1) {
        perror("errore handler");
        printf("Errore: %i", errno);

        return -1;
    }
    char buff[100] = "ciao";
    //leggiMessaggio(buff, 99, hand);
    /*
        while (1) {
            char stringa[30];
            puts("Invia messaggio:");
            inviaMessaggio(buff, hand);
            scanf("%s", stringa);
        }
     */

    //printf("%s\n", buff);

    chiudiFifo(path, hand, true);

    //File ServerFiglio.c in SysOp/Progetto

    int letti = 0;
    int pid, pidVecchio = 0;
    int counter = 0;



    while (1) {

        pid = atoi(buff);

        counter++;
        //if(pid != pidVecchio){

        printf("%i \t Byte letti %i \t Lettura numero %i\n", pid, letti, counter);
        //pidVecchio = pid;

        //}

        //		close(hand);
        //		hand = open(path, O_RDONLY);

    }




    //	unlink(path);





    return (0);
}