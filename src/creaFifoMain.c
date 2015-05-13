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
#include "parser.h"
#include "CONST.h"

void handler(int signo) {
    printf("Errore: %i\n", signo);
}

int main(int argc, char** argv) {


    messaggio x = messaggioConstructor();
    messaggio y = messaggioConstructor();


    //crInvDatiRisp(x, 200);
    crRichPartec(x, "/tmp/mioNom");
    //crAccettaClient(x);
    //crBroadAggPunti(x, 8, 21);
    //crBroadNuovoGiocatore(x, "Pippo", 8, 7);
    //crInvClassifica(x, "32,23,20,15,40");
    //crInvDomanda(x, 32, 46);
    //crInvLogOut(x);
    //crMesgCorrotto(x);
    //crRifiutaClient(x);

    //printf("\n%s\n", x.msg);

    int cont[3] = {1, 2, 3};
    leggiComm(y, x.msg, cont, 3);

    /*
     decodifica: 
     * da una stringa devo ottenere un Messaggio. 
     * Mando la lista dei miei contatti, se non lo conosco e non si vuole unire non leggo nemmeno
     * se lo conosco guardo il tipo del messaggio e in base a quello riempio la struct
     */

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
     
     //	unlink(path);
     */

    return (0);
}