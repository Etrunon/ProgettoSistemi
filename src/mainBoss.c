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
    //crRichPartec(x, "/tmp/mioNom");
    //crAccettaClient(x);
    //crBroadAggPunti(x, 8, 21);
    //crBroadNuovoGiocatore(x, "Pippo", 8, 7);
    //crInvClassifica(x, "32,23,20,15,40");
    //crInvDomanda(x, 33, 45);
    //crInvLogOut(x);
    //crMesgCorrotto(x);
    //crRifiutaClient(x);

    //printf("\n%s\n", x.msg);

    int cont[3] = {1, 2, 3};
    leggiComm(y, x.msg, cont, 3);

    return (0);
}