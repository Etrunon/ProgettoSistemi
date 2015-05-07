#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>



#include <linux/stat.h>

#include "creaFifo.h"

void handler(int signo) {
    printf("Errore: %i\n", signo);
}

int main(int argc, char** argv) {
    signal(SIGPIPE, handler);

    int hand = creaFifoScrittura("/tmp/testLibFifo");
    printf("%i\n", hand);


    char* stringa = "Ciao";
    write(hand, stringa, sizeof (stringa));

    int x = 0;
    scanf("%i", &x);

    write(hand, stringa, sizeof (stringa));


    int letti = 0;
    int pid, pidVecchio = 0;
    int counter = 0;

    /*
     
     while (1) {
     
     char buff[100];
     letti = read(hand, buff, 99);
     
     
     pid = atoi(buff);
     
     counter++;
     //if(pid != pidVecchio){
     
     printf("%i \t Byte letti %i \t Lettura numero %i\n", pid, letti, counter);
     //pidVecchio = pid;
     
     
     //}
     
     
     //		close(hand);
     //		hand = open(path, O_RDONLY);
     
     }
     */



    //	unlink(path);

    return (0);
}