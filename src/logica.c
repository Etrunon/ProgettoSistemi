#include <time.h>
#include <stdlib.h>

#include "logica.h"

domanda* generaDomanda() {

    domanda dom;
    srand(time(NULL));
    dom.numero1 = rand();
    dom.numero2 = rand();

    printf("%i, %i\n", dom.numero1, dom.numero2);
    return dom;
}

void rispostaDelCLient(messaggio *msg) {

}

/**
 *
 * @param msg
 */
void ingressoDelClient(messaggio *msg) {

}

/**
 *
 * @param msg
 */
void processaRisposta(messaggio msg) {

    switch (msg.codiceMsg) {
        case 1: rispostaDelCLient(&msg);
            break;
        case 2: ingressoDelClient(&msg);
            break;
        case 3: //InvLogOut;
            break;
        default: break;
    }
}