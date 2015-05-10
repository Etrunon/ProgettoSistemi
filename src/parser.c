#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

#include "parser.h"

/**
 * Funzione che data una STRINGA di 19 celle ritorna il timestamp come stringa seguito da \0
 * @return 
 */
void timestamp(char* s) {

    time_t t = time(NULL);
    struct tm * p = localtime(&t);
    strftime(s, 25, "%y %m %d %H:%M:%S", p);

}

bool headerMsg(messaggio x) {

    int pid = getpid();
    char* strTime = (char*) malloc(25 * sizeof (char));
    timestamp(strTime);

    sprintf(x.msg, "%i!%s", pid, strTime);

    free(strTime);
    return true;
}

/*
 TODO Sistemare chiamata qui alla struct. Parser.c non conosce la dimensione della variabile messaggio->messFinale[] dichiarata
 *      nel file parser.h
 */
bool crInvDatiRisp(messaggio x, int rispo) {

    headerMsg(x);

    char* tmp = (char*) malloc(7 * sizeof (char));

    sprintf(tmp, "!ri!%i", rispo);
    strcat(x.msg, tmp);

    free(tmp);
    return true;
}

bool crRichPartec(messaggio x, char* pathFifo) {

    headerMsg(x);
    int spazioDisponibile = MSG_SIZE - strlen(x.msg);
    char* tmp = (char*) malloc(spazioDisponibile * sizeof (char));

    sprintf(tmp, "!jn!%s", pathFifo);

    if (strlen(tmp) >= spazioDisponibile) {
        return NULL;
    }
    strcat(x.msg, tmp);

    free(tmp);

}

bool crInvLogOut(messaggio x) {

    headerMsg(x);

    char* tmp = (char*) malloc(3 * sizeof (char));

    sprintf(tmp, "!lo");
    strcat(x.msg, tmp);

    free(tmp);

};

bool crMesgCorrotto(messaggio x) {
    headerMsg(x);

    char* tmp = (char*) malloc(3 * sizeof (char));

    sprintf(tmp, "!co");
    strcat(x.msg, tmp);

    free(tmp);
};

bool crAccettaClient(messaggio x) {

    headerMsg(x);

    char* tmp = (char*) malloc(3 * sizeof (char));

    sprintf(tmp, "!ok");
    strcat(x.msg, tmp);

    free(tmp);
}

bool crRifiutaClient(messaggio x) {
    headerMsg(x);

    char* tmp = (char*) malloc(3 * sizeof (char));

    sprintf(tmp, "!no");
    strcat(x.msg, tmp);

    free(tmp);
}

bool crInvClassifica(messaggio x, char* classifica) {
    headerMsg(x);

    int spazioDisponibile = MSG_SIZE - strlen(x.msg);
    char* tmp = (char*) malloc(spazioDisponibile * sizeof (char));

    sprintf(tmp, "!cl!%s", classifica);

    if (strlen(tmp) >= spazioDisponibile) {
        return NULL;
    }

    strcat(x.msg, tmp);

    free(tmp);
}

bool crBroadNuovoGiocatore(messaggio x, char* nome, int codice, int punti) {

    headerMsg(x);

    int spazioDisponibile = MSG_SIZE - strlen(x.msg);
    char* tmp = (char*) malloc(spazioDisponibile * sizeof (char));

    sprintf(tmp, "!np!%s!%i!%i", nome, codice, punti);

    if (strlen(tmp) >= spazioDisponibile) {
        return NULL;
    }
    strcat(x.msg, tmp);

    free(tmp);
}

bool crBroadAggPunti(messaggio x, int codice, int punti) {

    headerMsg(x);

    char* tmp = (char*) malloc(12 * sizeof (char));

    sprintf(tmp, "!ap!%i!%i", codice, punti);
    strcat(x.msg, tmp);

    free(tmp);
}

bool crInvDomanda(messaggio x, int primoNum, int secondoNum) {
    headerMsg(x);

    char* tmp = (char*) malloc(11 * sizeof (char));

    sprintf(tmp, "!ap!%i!%i", primoNum, secondoNum);
    strcat(x.msg, tmp);

    free(tmp);
}

