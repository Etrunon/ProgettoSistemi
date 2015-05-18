#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>

#include "riparser.h"
#include "CONST.h"

messaggio* messaggioConstructor() {

    messaggio *msg = (messaggio*) malloc(sizeof (messaggio));

    msg->pidMit = -1;
    msg->codiceMsg = -1;
    msg->timestring = (char*) malloc(25 * (sizeof (char)));
    msg->msg = (char*) malloc(MSG_SIZE * (sizeof (char)));
    msg->pathFifo = (char*) malloc(MAX_FIFONAME * (sizeof (char)));
    msg->nomeClient = (char*) malloc(MAXNAME * (sizeof (char)));
    msg->numeroClient = -1;
    msg->valRisposta = -1;
    msg->clientID = -1;
    msg->clientPunti = -1;
    msg->domandaNum1 = -1;
    msg->domandaNum2 = -1;
    msg->corretta = false;

    return msg;
}

void messaggioDestructor(messaggio *x) {

    if (x->timestring != NULL) {
        free(x->timestring);
    }
    if (x->msg != NULL) {
        free(x->msg);
    }
    if (x->pathFifo != NULL) {
        free(x->pathFifo);
    }
    if (x->nomeClient != NULL) {
        free(x->nomeClient);
    }
    free(x);
}

void testStampaMessaggio(messaggio *msg, char* testo) {
    /*
     * TODO creare la stampa "formattata" per fare debrugging
     */
    printf("Messaggio: \n %s \n\t ClientID %i\n", testo, msg->clientID);
    printf("\t Codice msg: %i\n", msg->codiceMsg);
    printf("\t StringaFin.: %s\n", msg->msg);
    printf("\t Val. Risposta: %i\n", msg->valRisposta);
    printf("\t Path Fifo: %s\n", msg->pathFifo);
    printf("\t Nome Client: %s \n", msg->nomeClient);
    printf("\t PID: %i\n", msg->pidMit);

}

/**
 * Funzione che concatena un intero a una stringa formata in maniera corretta.
 * Necessario l'indirizzo della stringa su cui concatenare
 * @param conc
 */
void concatInt(char* str, int x) {

    char tmp[25];
    memset(tmp, 0, 25);

    sprintf(tmp, "%i%c", x, 31);
    strcat(str, tmp);
}

/**
 * Funzione che concatena una stringa una stringa formata in maniera corretta.
 * Necessario l'indirizzo della stringa su cui concatenare
 * @param conc
 */
void concatStr(char* str, char* cnc) {

    char tmp[MSG_SIZE];
    memset(tmp, 0, MSG_SIZE);

    int last = strlen(cnc) - 1;

    if (cnc[last] == 31) {
        sprintf(tmp, "%s", cnc);
    } else
        sprintf(tmp, "%s%c", cnc, 31);

    strcat(str, tmp);

}

void timestamp(char* s) {

    time_t t = time(NULL);
    struct tm * p = localtime(&t);
    strftime(s, 25, "%y-%m-%d %H:%M:%S", p);

}

void headerMsg(messaggio *msg) {

    concatInt(msg->msg, msg->clientID);

    char *time = (char*) malloc(25 * (sizeof (char)));
    timestamp(time);
    concatStr(msg->msg, time);

    concatInt(msg->msg, msg->codiceMsg);
}

bool creaMessaggio(messaggio *mess) {

    headerMsg(mess);
    //printf("%s\n", mess->msg);

    switch (mess->codiceMsg) {
        case 1:
        {
            concatInt(mess->msg, mess->valRisposta);
        };
            break;
        case 2:
        {
            concatStr(mess->msg, mess->pathFifo);
            concatStr(mess->msg, mess->nomeClient);
            concatInt(mess->msg, getpid());
        };
            break;
        case 3: //Invio informazione logout al server
            break;
        case 4:
        {
            concatInt(mess->msg, mess->numeroClient);
        }
            break;
        case 5: //Client rifiutato
            break;
        case 6:
        {
            concatStr(mess->msg, mess->pathFifo);
            concatStr(mess->msg, mess->nomeClient);
            /*
             concatInt(mess->msg, mess->clientID);
             TODO aggiungere ID soggetto
             */
        }
            break;
        case 7:
        {
            concatInt(mess->msg, mess->clientID);
        }
            break;
        case 8:
        {
            concatInt(mess->msg, mess->clientID);
            concatInt(mess->msg, mess->clientPunti);
        }
            break;
        case 9:
        {

            if (mess->corretta == true)
                concatInt(mess->msg, 1);
            else
                concatInt(mess->msg, 0);
        }
            break;
        case 10:
        {
            concatInt(mess->msg, mess->domandaNum1);
            concatInt(mess->msg, mess->domandaNum2);
        }
            break;
        case 11: //Server in spegnimento
            break;

    }

    testStampaMessaggio(mess, "Alla fine del crea");

    return true;
}

char* prossimoSeparatore(char* str) {
    char*pp = NULL;
    pp = strchr(str, 31);

    return pp;
}

void decatStr(char **str, char **dest) {

    char* mezza = prossimoSeparatore(*str);

    memcpy(*dest, *str, mezza - *str);

    *str = mezza + 1;
}

void decatInt(char **str, int **dest) {

    char* mezza = prossimoSeparatore(*str);
    char *tmp = (char*) malloc(25 * (sizeof (char)));
    memset(tmp, 0, 25);

    memcpy(tmp, *str, mezza - *str);

    **dest = strtol(tmp, NULL, 10);

    *str = mezza + 1;

    free(tmp);
}

void traduciHeader(messaggio *msg, char **lettaFino) {

    char *tmp = (char*) malloc(MSG_SIZE * (sizeof (char)));
    memset(tmp, 0, MSG_SIZE);

    int *x = (int*) malloc(1 * (sizeof (int)));

    decatInt(lettaFino, &x);

    msg->clientID = *x;

    decatStr(lettaFino, &tmp);
    strcpy(msg->timestring, tmp);
    memset(tmp, 0, MSG_SIZE);

    decatInt(lettaFino, &x);
    msg->codiceMsg = *x;

    free(tmp);
    free(x);

}

void traduciComm(messaggio *msg) {

    char *lettaFino = msg->msg;
    traduciHeader(msg, &lettaFino);

    char *tmp = (char*) malloc(MSG_SIZE * (sizeof (char)));
    memset(tmp, 0, MSG_SIZE);

    int *x = (int*) malloc(1 * (sizeof (int)));

    switch (msg->codiceMsg) {
        case 1:
        {
            decatInt(&lettaFino, &x);
            msg->valRisposta = *x;
        };
            break;
        case 2:
        {
            testStampaMessaggio(msg, "Dentro lo switch");
            decatStr(&lettaFino, &tmp);
            strcpy(msg->pathFifo, tmp);

            decatStr(&lettaFino, &tmp);
            strcpy(msg->nomeClient, tmp);

            decatInt(&lettaFino, &x);
            msg->pidMit = *x;
        };
            break;
        case 3: //Invio informazione logout al server
            break;
        case 4:
        {
            /*
             * TODO aggiungere il numero client
             decatInt(&lettaFino, &tmp);
             strcpy(msg->pidMit, &x);
             msg->numeroClient = *x;

             */
        }
            break;
        case 5: //Client rifiutato
            break;
        case 6:
        {
            concatStr(msg->msg, msg->pathFifo);
            concatStr(msg->msg, msg->nomeClient);
            concatInt(msg->msg, msg->valRisposta);
        }
            break;
        case 7:
        {
            concatInt(msg->msg, msg->clientID);
        }
            break;
        case 8:
        {
            concatInt(msg->msg, msg->clientID);
            concatInt(msg->msg, msg->clientPunti);
        }
            break;
        case 9:
        {

            if (msg->corretta == true)
                concatInt(msg->msg, 1);
            else
                concatInt(msg->msg, 0);
        }
            break;
        case 10:
        {
            concatInt(msg->msg, msg->domandaNum1);
            concatInt(msg->msg, msg->domandaNum2);
        }
            break;
        case 11: //Server in spegnimento
            break;


    }

    free(tmp);
    free(x);

}

