#include <time.h>
#include <stdlib.h>

#include "logica.h"

classifica classificaConstructor() {
    classifica cla;

    cla.listaNomi = (char**) malloc(NUM_CLIENT * (sizeof (char*)));
    cla.listaPunti = (int*) malloc(NUM_CLIENT * (sizeof (int)));
    cla.clientAttivi = 0;
    return cla;
}

void classificaDestructor(classifica cla) {

    free(cla.listaPunti);
    int i = 0;

    for (i; i < cla.clientAttivi; i++) {
        free(cla.listaNomi[i]);
    }

    free(cla.listaNomi);
}

partita partitaConstructor() {

    partita par;
    domanda domanda;
    par.classifica = (classifica*) malloc(sizeof (classifica));
    par.classifica = classificaConstructor();

    return par;
}

void partitaDestructor(partita par) {
    classificaDestructor(par.classifica);
    free(par.classifica);
}

domanda generaDomanda() {

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