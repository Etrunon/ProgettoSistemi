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
#include "CONST.h"
#include "logica.h"

void handler(int signo) {
    printf("Errore: %i\n", signo);
}

void testPrintTuttiMessaggi() {
    int handler = 1;
    handler = creaFiFoScrittura("/tmp/miaFifo");
    messaggio *mittando = messaggioConstructor();

    //crInvDatiRisp(x, 200);
    mittando->codiceMsg = 1;
    mittando->valRisposta = 200;
    mittando->codiceMsg = 1;

    inviaMessaggio(handler, mittando);
    messaggioDestructor(mittando);
    mittando = messaggioConstructor();


    //crRichPartec(x, "/tmp/mioNom");
    mittando->codiceMsg = 2;
    mittando->pathFifo = (char*) malloc(40 * (sizeof (char)));
    sprintf(mittando->pathFifo, "%s", "/tmp/path");

    inviaMessaggio(handler, mittando);
    messaggioDestructor(mittando);
    mittando = messaggioConstructor();

    //crAccettaClient(x);
    mittando->codiceMsg = 5;

    inviaMessaggio(handler, mittando);
    messaggioDestructor(mittando);
    mittando = messaggioConstructor();

    //crBroadAggPunti(x, 8, 21);
    mittando->codiceMsg = 9;
    mittando->clientID = 5;
    mittando->clientPunti = 34;

    inviaMessaggio(handler, mittando);
    messaggioDestructor(mittando);
    mittando = messaggioConstructor();

    //crBroadNuovoGiocatore(x, "Pippo", 8, 7);
    mittando->codiceMsg = 8;
    mittando->nomeClient = (char*) malloc(40 * (sizeof (char)));
    sprintf(mittando->nomeClient, "%s", "pippo");

    mittando->clientID = 3;
    mittando->clientPunti = 56;

    inviaMessaggio(handler, mittando);
    messaggioDestructor(mittando);
    mittando = messaggioConstructor();

    //crInvDomanda(x, 33, 45);
    mittando->codiceMsg = 10;
    mittando->domandaNum1 = 87;
    mittando->domandaNum2 = 45;

    inviaMessaggio(handler, mittando);
    messaggioDestructor(mittando);
    mittando = messaggioConstructor();

    //crInvLogOut(x);
    mittando->codiceMsg = 3;

    inviaMessaggio(handler, mittando);
    messaggioDestructor(mittando);
    mittando = messaggioConstructor();

    //crMesgCorrotto(x);
    mittando->codiceMsg = 4;

    inviaMessaggio(handler, mittando);
    messaggioDestructor(mittando);
    mittando = messaggioConstructor();

    //crRifiutaClient(x);
    mittando->codiceMsg = 6;

    inviaMessaggio(handler, mittando);
    messaggioDestructor(mittando);

}

void testInviaMessaggiAlServer() {

    int handler = 1, imax = 1;
    handler = creaFiFoScrittura(SERVERPATH);

    messaggio *mittando = messaggioConstructor();

    //crInvDatiRisp(x, 200);
    mittando->codiceMsg = 1;
    mittando->valRisposta = 200;
    mittando->codiceMsg = 1;

    int i = 0;
    for (i; i < imax; i++) {
        inviaMessaggio(handler, mittando);
        sleep(1);
    }
    messaggioDestructor(mittando);
    mittando = messaggioConstructor();


    //crRichPartec(x, "/tmp/mioNom");
    mittando->codiceMsg = 2;
    mittando->pathFifo = (char*) malloc(40 * (sizeof (char)));
    sprintf(mittando->pathFifo, "%s", "/tmp/path");

    i = 0;
    for (i; i < imax; i++) {
        inviaMessaggio(handler, mittando);
        sleep(1);
    }
    messaggioDestructor(mittando);
    mittando = messaggioConstructor();

    //crAccettaClient(x);
    mittando->codiceMsg = 5;

    i = 0;
    for (i; i < imax; i++) {
        inviaMessaggio(handler, mittando);
        sleep(1);
    }
    messaggioDestructor(mittando);
    mittando = messaggioConstructor();

    //crBroadAggPunti(x, 8, 21);
    mittando->codiceMsg = 9;
    mittando->clientID = 5;
    mittando->clientPunti = 34;

    i = 0;
    for (i; i < imax; i++) {
        inviaMessaggio(handler, mittando);
        sleep(1);
    }
    messaggioDestructor(mittando);
    mittando = messaggioConstructor();

    //crBroadNuovoGiocatore(x, "Pippo", 8, 7);
    mittando->codiceMsg = 8;
    mittando->nomeClient = (char*) malloc(40 * (sizeof (char)));
    sprintf(mittando->nomeClient, "%s", "pippo");

    mittando->clientID = 3;
    mittando->clientPunti = 56;

    i = 0;
    for (i; i < imax; i++) {
        inviaMessaggio(handler, mittando);
        sleep(1);
    }
    messaggioDestructor(mittando);
    mittando = messaggioConstructor();

    //crInvDomanda(x, 33, 45);
    mittando->codiceMsg = 10;
    mittando->domandaNum1 = 87;
    mittando->domandaNum2 = 45;

    i = 0;
    for (i; i < imax; i++) {
        inviaMessaggio(handler, mittando);
        sleep(1);
    }
    messaggioDestructor(mittando);
    mittando = messaggioConstructor();

    //crInvLogOut(x);
    mittando->codiceMsg = 3;

    i = 0;
    for (i; i < imax; i++) {
        inviaMessaggio(handler, mittando);
        sleep(1);
    }
    messaggioDestructor(mittando);
    mittando = messaggioConstructor();

    //crMesgCorrotto(x);
    mittando->codiceMsg = 4;

    i = 0;
    for (i; i < imax; i++) {
        inviaMessaggio(handler, mittando);
        sleep(1);
    }
    messaggioDestructor(mittando);
    mittando = messaggioConstructor();

    //crRifiutaClient(x);
    mittando->codiceMsg = 6;

    i = 0;
    for (i; i < imax; i++) {
        inviaMessaggio(handler, mittando);
        sleep(1);
    }
    messaggioDestructor(mittando);

}

int main(int argc, char** argv) {

    testInviaMessaggiAlServer();

    return (0);
}