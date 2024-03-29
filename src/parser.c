/*
 * Progetto: Multiplayer Game
 * A.A 2014/2015
 * Carlo Mion   165878
 * Luca Bosotti 164403
 */

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "parser.h"
#include "CONST.h"
#include "commands.h"
#include "guiMessages.h"

messaggio* messaggioConstructor(int IDMittente, tipoMessaggio tipoMsg) {

    messaggio *msg = (messaggio*) malloc(sizeof (messaggio));

    msg->PIDMittente = getpid();
    msg->codiceMsg = tipoMsg;
    msg->timestring = (char*) malloc(25 * (sizeof (char)));
    memset(msg->timestring, 0, 25);

    msg->msg = (char*) malloc(MSG_SIZE * (sizeof (char)));
    memset(msg->msg, 0, MSG_SIZE);

    msg->pathFifo = (char*) malloc(MAX_FIFONAME * (sizeof (char)));
    memset(msg->pathFifo, 0, MAX_FIFONAME);

    msg->nomeClient = (char*) malloc(MAXNAME * (sizeof (char)));
    memset(msg->nomeClient, 0, MAXNAME);

    msg->risposta = -1;
    msg->IDMittente = IDMittente;
    msg->punti = -1;
    msg->domanda1 = -1;
    msg->domanda2 = -1;
    msg->corretta = false;
    msg->IDOggetto = -1;

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

/*Usata nel debugging della FIFO*/
void testStampaMessaggio(messaggio *msg, char* testo) {
    /*
     * TODO creare la stampa "formattata" per fare debrugging
     */
    char tmp [BUFFMESSAGGIO];

    sprintf(tmp, "\n%s%s%s\n", "----------", testo, "----------");
    aggiungiMessaggio(tmp, false, ANSI_COLOR_RED);

    sprintf(tmp, "%s IDMittente:%i\n", testo, msg->IDMittente);
    aggiungiMessaggio(tmp, false, NULL);

    sprintf(tmp, "CodiceMessaggio:%i\n", msg->codiceMsg);
    aggiungiMessaggio(tmp, false, NULL);

    sprintf(tmp, "Stringa:%s\n", msg->msg);
    aggiungiMessaggio(tmp, false, NULL);

    sprintf(tmp, "Risposta:%i\n", msg->risposta);
    aggiungiMessaggio(tmp, false, NULL);

    sprintf(tmp, "Punti:%i\n", msg->punti);
    aggiungiMessaggio(tmp, false, NULL);

    sprintf(tmp, "IDOggetto:%i\n", msg->IDOggetto);
    aggiungiMessaggio(tmp, false, NULL);

    sprintf(tmp, "NomeClient:%s\n", msg->nomeClient);
    aggiungiMessaggio(tmp, false, NULL);

    sprintf(tmp, "-------------Fine Messaggio----------\n");
    aggiungiMessaggio(tmp, false, ANSI_COLOR_BLUE);

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

/*Crea l'header del messagio*/
void headerMsg(messaggio *msg) {

    concatInt(msg->msg, msg->IDMittente);

    char *time = (char*) malloc(25 * (sizeof (char)));
    timestamp(time);
    concatStr(msg->msg, time);

    concatInt(msg->msg, msg->codiceMsg);
}

bool creaMessaggio(messaggio *mess) {

    headerMsg(mess);
    //printf("%s\n", mess->msg);

    switch (mess->codiceMsg) {
        case INVIA_RISPOSTA:
        {
            concatInt(mess->msg, mess->risposta);
        };
            break;
        case RICHIESTA_PARTECIPAZIONE:
        {
            concatStr(mess->msg, mess->pathFifo);
            concatStr(mess->msg, mess->nomeClient);
            concatInt(mess->msg, getpid());
        };
            break;
        case LOGOUT_AL_SERVER: //Invio informazione logout al server
            break;
        case ACCETTA_CLIENT:
        {
            concatInt(mess->msg, mess->IDOggetto);
            concatInt(mess->msg, mess->punti);
            concatInt(mess->msg, mess->maxWin);
        }
            break;
        case RIFIUTA_CLIENT: //Client rifiutato
            break;
        case NUOVO_GIOCATORE_ENTRATO:
        {
            concatStr(mess->msg, mess->nomeClient);
            concatInt(mess->msg, mess->IDOggetto);
            concatInt(mess->msg, mess->punti);
        }
            break;
        case GIOCATORE_USCITO:
        {
            concatInt(mess->msg, mess->IDOggetto);
        }
            break;
        case MODIFICA_PUNTEGGIO_GIOCATORE:
        {
            concatInt(mess->msg, mess->IDOggetto);
            concatInt(mess->msg, mess->punti);
            concatInt(mess->msg, mess->corretta);
        }
            break;
        case ESITO_RISPOSTA:
        {
            if (mess->corretta == true)
                concatInt(mess->msg, 1);
            else
                concatInt(mess->msg, 0);

            concatInt(mess->msg, mess->punti);
        }
            break;
        case INVIA_DOMANDA:
        {
            concatInt(mess->msg, mess->domanda1);
            concatInt(mess->msg, mess->domanda2);
        }
            break;
        case SERVER_SPEGNIMENTO: //Server in spegnimento
            break;
        case VITTORIA:
        {
            concatInt(mess->msg, mess->IDOggetto);
        }
            break;
    }

    //testStampaMessaggio(mess, "Alla fine del crea");

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

    msg->IDMittente = *x;

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
        case INVIA_RISPOSTA:
        {
            decatInt(&lettaFino, &x);
            msg->risposta = *x;
        };
            break;
        case RICHIESTA_PARTECIPAZIONE:
        {
            decatStr(&lettaFino, &tmp);
            strcpy(msg->pathFifo, tmp);
            memset(tmp, 0, MSG_SIZE);

            decatStr(&lettaFino, &tmp);
            strcpy(msg->nomeClient, tmp);

            decatInt(&lettaFino, &x);
            msg->PIDMittente = *x;
        };
            break;
        case LOGOUT_AL_SERVER: //Invio informazione logout al server
            break;
        case ACCETTA_CLIENT:
        {
            decatInt(&lettaFino, &x);
            msg->IDOggetto = *x;
            decatInt(&lettaFino, &x);
            msg->punti = *x;
            decatInt(&lettaFino, &x);
            msg->maxWin = *x;
        }
            break;
        case RIFIUTA_CLIENT: //Client rifiutato
            break;
        case NUOVO_GIOCATORE_ENTRATO:
        {
            decatStr(&lettaFino, &tmp);
            strcpy(msg->nomeClient, tmp);
            memset(tmp, 0, MSG_SIZE);
            decatInt(&lettaFino, &x);
            msg->IDOggetto = *x;
            decatInt(&lettaFino, &x);
            msg->punti = *x;
        }
            break;
        case GIOCATORE_USCITO:
        {
            decatInt(&lettaFino, &x);
            msg->IDOggetto = *x;
        }
            break;
        case MODIFICA_PUNTEGGIO_GIOCATORE:
        {
            decatInt(&lettaFino, &x);
            msg->IDOggetto = *x;
            decatInt(&lettaFino, &x);
            msg->punti = *x;
            decatInt(&lettaFino, &x);
            msg->corretta = *x;
        }
            break;
        case ESITO_RISPOSTA:
        {
            decatInt(&lettaFino, &x);
            if (*x == 1) {

                msg->corretta = true;
            } else
                msg->corretta = false;

            decatInt(&lettaFino, &x);
            msg->punti = *x;
        }
            break;
        case INVIA_DOMANDA:
        {
            decatInt(&lettaFino, &x);
            msg->domanda1 = *x;
            decatInt(&lettaFino, &x);
            msg->domanda2 = *x;
        }
            break;
        case SERVER_SPEGNIMENTO: //Server in spegnimento
            break;
        case VITTORIA:
        {
            decatInt(&lettaFino, &x);
            msg->IDOggetto = *x;
        }
            break;

    }

    free(tmp);
    free(x);

}

