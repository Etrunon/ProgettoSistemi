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

void testPrint(messaggio *pm) {
    inviaMessaggio(1, pm);
    messaggioDestructor(pm);
    //pm = messaggioConstructor();
}

int main(int argc, char** argv) {


    messaggio *mittando = messaggioConstructor();
    //messaggio *mittuto = messaggioConstructor();


    //crInvDatiRisp(x, 200);
    mittando->codiceMsg = 1;
    mittando->valRisposta = 20000000000;
    mittando->codiceMsg = 1;
    //    inviaMessaggio(1, mittando);
    testPrint(mittando);



    //crRichPartec(x, "/tmp/mioNom");
    mittando->codiceMsg = 2;
    mittando->pathFifo = (char*) malloc(40 * (sizeof (char)));
    sprintf(mittando->pathFifo, "%s", "/tmp/path");
    testPrint(mittando);
    /*
     //crAccettaClient(x);
     mittando->codiceMsg = 5;
     
     testPrint(mittando);
     
     //crBroadAggPunti(x, 8, 21);
     mittando->codiceMsg = 9;
     mittando->clientID = 5;
     mittando->clientPunti = 34;
     
     testPrint(mittando);
     
     //crBroadNuovoGiocatore(x, "Pippo", 8, 7);
     mittando->codiceMsg = 8;
     mittando->nomeClient = (char*) malloc(40 * (sizeof (char)));
     mittando->nomeClient = "pippo";
     mittando->clientID = 3;
     mittando->clientPunti = 56;
     
     testPrint(mittando);
     
     //crInvDomanda(x, 33, 45);
     mittando->codiceMsg = 10;
     mittando->domandaNum1 = 87;
     mittando->domandaNum2 = 45;
     
     testPrint(mittando);
     
     //crInvLogOut(x);
     mittando->codiceMsg = 3;
     
     testPrint(mittando);
     
     //crMesgCorrotto(x);
     mittando->codiceMsg = 4;
     
     testPrint(mittando);
     
     //crRifiutaClient(x);
     mittando->codiceMsg = 6;
     
     testPrint(mittando);
     
     
     //creaRisposta(&mittando);
     //inviamessaggio(mittando.msg, handlerFifo)
     
     //mittuto.msg = leggiMessaggio(dalla fifo);
     //leggiComm(mittuto);
     //    leggiComm(mittuto, mittando.msg, 3);
     */

    return (0);
}