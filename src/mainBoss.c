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
#include "logica.h"
#include "CONST.h"

void handler(int signo) {
    printf("Errore: %i\n", signo);
}

int main(int argc, char** argv) {

    /*
     messaggio mittando = messaggioConstructor();
     messaggio mittuto = messaggioConstructor();
     
     
     //crInvDatiRisp(x, 200);
     mittando.valRisposta = 200;
     mittando.codiceMsg = 1;
     //crRichPartec(x, "/tmp/mioNom");
     //crAccettaClient(x);
     //crBroadAggPunti(x, 8, 21);
     //crBroadNuovoGiocatore(x, "Pippo", 8, 7);
     //crInvClassifica(x, "32,23,20,15,40");
     //crInvDomanda(x, 33, 45);
     //crInvLogOut(x);
     //crMesgCorrotto(x);
     //crRifiutaClient(x);
     
     //creaRisposta(&mittando);
     //inviamessaggio(mittando.msg, handlerFifo)
     
     //mittuto.msg = leggiMessaggio(dalla fifo);
     //leggiComm(mittuto);
     leggiComm(mittuto, mittando.msg, 3);
     */

    domanda dom = generaDomanda();

    printf("%i, %i\n", dom.numero1, dom.numero2);

    return (0);
}