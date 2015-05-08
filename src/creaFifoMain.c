#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/stat.h>

#include <stdio.h> 

#include <time.h> 

#include "allFifo.h"

void handler(int signo) {
    printf("Errore: %i\n", signo);
}

int main(int argc, char** argv) {

    /*
     //handler dei SIGNAL
     signal(SIGPIPE, handler);
     
     char* path = "/tmp/testLibFifo";
     
     //Test LibreriaFIFO
     int hand = creaFifoLettura(path);
     
     
     char buff[100];
     leggiMessaggio(buff, 99, hand);
     printf("%s\n", buff);
     
     chiudiFifo(path, true);
     
     //File ServerFiglio.c in SysOp/Progetto 
     
     int letti = 0;
     int pid, pidVecchio = 0;
     int counter = 0;
     
     /*
     
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
     
     * 
     
     
     
     //	unlink(path);
     */





    char s[1000];

    time_t t = time(NULL);

    struct tm * p = localtime(&t);

    strftime(s, 1000, "%A%B%d%Y", p);

    printf("[%s]\n", s);




    return (0);
}