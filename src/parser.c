#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

#include "parser.h"

/**
 * Costruttore della struct messaggio
 * @return 
 */
messaggio messaggioConstructor() {

    messaggio msg;
    msg.pidMit = -1;
    msg.codiceMsg = -1;
    msg.timestamp = NULL;
    msg.timestring = NULL;
    msg.msg = (char*) malloc(MSG_SIZE * (sizeof (char)));
    msg.pathFifo = NULL;
    msg.nomeClient = NULL;
    msg.classifica = NULL;
    msg.numeroClient = -1;
    msg.valRisposta = -1;
    msg.clientID = -1;
    msg.clientPunti = -1;
    msg.domandaNum1 = -1;
    msg.domandaNum2 = -1;

    return msg;
}

/**
 * Distruttore della classe messaggio
 * @param s
 */
void messaggioDestructor(messaggio x) {

    if (x.timestamp != NULL) {
        free(x.timestamp);
    }
    if (x.timestring != NULL) {
        free(x.timestring);
    }
    if (x.msg != NULL) {
        free(x.msg);
    }
    if (x.pathFifo != NULL) {
        free(x.pathFifo);
    }
    if (x.nomeClient != NULL) {
        free(x.nomeClient);
    }
    if (x.classifica != NULL) {
        free(x.classifica);
    }
}

/**
 * Funzione che data una STRINGA di 19 celle ritorna il timestamp come stringa seguito da \0
 * @return 
 */
void timestamp(char* s) {

    time_t t = time(NULL);
    struct tm * p = localtime(&t);
    strftime(s, 25, "%y-%m-%d %H:%M:%S", p);

}

bool headerMsg(messaggio *x) {

    int pid = getpid();
    char* strTime = (char*) malloc(25 * sizeof (char));
    timestamp(strTime);

    sprintf(x->msg, "%i!%s", pid, strTime);

    free(strTime);
    return true;
}

bool crInvDatiRisp(messaggio x, int rispo) {

    headerMsg(&x);

    char* tmp = (char*) malloc(7 * sizeof (char));

    sprintf(tmp, "!1!%i", rispo);
    strcat(x.msg, tmp);

    //sprintf(x.msg, "%s%s", x.msg, tmp);

    free(tmp);
    return true;
}

bool crRichPartec(messaggio x, char* pathFifo) {

    headerMsg(&x);
    int spazioDisponibile = MSG_SIZE - strlen(x.msg);
    char* tmp = (char*) malloc(spazioDisponibile * sizeof (char));

    sprintf(tmp, "!2!%s", pathFifo);

    if (strlen(tmp) >= spazioDisponibile) {
        return NULL;
    }
    strcat(x.msg, tmp);

    free(tmp);

}

bool crInvLogOut(messaggio x) {

    headerMsg(&x);

    char* tmp = (char*) malloc(3 * sizeof (char));

    sprintf(tmp, "!3");
    strcat(x.msg, tmp);

    free(tmp);

};

bool crMesgCorrotto(messaggio x) {
    headerMsg(&x);

    char* tmp = (char*) malloc(3 * sizeof (char));

    sprintf(tmp, "!4");
    strcat(x.msg, tmp);

    free(tmp);
};

bool crAccettaClient(messaggio x) {

    headerMsg(&x);

    char* tmp = (char*) malloc(3 * sizeof (char));

    sprintf(tmp, "!5");
    strcat(x.msg, tmp);

    free(tmp);
}

bool crRifiutaClient(messaggio x) {
    headerMsg(&x);

    char* tmp = (char*) malloc(3 * sizeof (char));

    sprintf(tmp, "!6");
    strcat(x.msg, tmp);

    free(tmp);
}

/**
 * TODO Eliminare? Per ragioni di lunghezza stringa
 * @param x
 * @return 
 */
bool crInvClassifica(messaggio x, char *classifica) {
    headerMsg(&x);

    int spazioDisponibile = MSG_SIZE - strlen(x.msg);
    char* tmp = (char*) malloc(spazioDisponibile * sizeof (char));

    sprintf(tmp, "!7!%s", classifica);

    if (strlen(tmp) >= spazioDisponibile) {
        return NULL;
    }

    strcat(x.msg, tmp);

    free(tmp);
}

bool crBroadNuovoGiocatore(messaggio x, char* nome, int codice, int punti) {

    headerMsg(&x);

    int spazioDisponibile = MSG_SIZE - strlen(x.msg);
    char* tmp = (char*) malloc(spazioDisponibile * sizeof (char));

    sprintf(tmp, "!8!%s!%i!%i", nome, codice, punti);

    if (strlen(tmp) >= spazioDisponibile) {
        return NULL;
    }
    strcat(x.msg, tmp);

    free(tmp);
}

bool crBroadAggPunti(messaggio x, int codice, int punti) {

    headerMsg(&x);

    char* tmp = (char*) malloc(12 * sizeof (char));

    sprintf(tmp, "!9!%i!%i", codice, punti);
    strcat(x.msg, tmp);

    free(tmp);
}

bool crInvDomanda(messaggio x, int primoNum, int secondoNum) {
    headerMsg(&x);

    char* tmp = (char*) malloc(11 * sizeof (char));

    sprintf(tmp, "!10!%i!%i", primoNum, secondoNum);
    strcat(x.msg, tmp);

    free(tmp);
}

//--------------- DECODIFICA

bool checkPid(int*contatti, int dim, int pid) {
    return true;
    return false;
}

bool decInvDatiRisp(messaggio *x) {

    char *check = NULL;

    //Escludo il separatore '!'
    (x->msg)++;
    x->valRisposta = strtol(x->msg, &check, 10);

    if (check == NULL)
        return true;
    else
        return false;
}

/**
 * 
 * @return 
 */
bool decRichPartec(messaggio *x) {

    (x->msg)++;

    int lgMax = MSG_SIZE - 26 - strlen(x->msg), lgPath = strlen(x->msg);

    if (lgMax <= lgPath) {
        printf("Sono nell'if che controlla la lunghezza");
        return false;
    }

    x->pathFifo = (char*) malloc(lgPath * (sizeof (char)));

    sprintf(x->pathFifo, "%s%c", x->msg, '\0');
    x->pathFifo[(lgPath + 1)] = '\0';

    return true;
}

/**
 * 
 * @return 
 */
bool decBroadNuovoGiocatore(messaggio *x) {

    char * aliasMsg = x->msg + 1;

    //Raccolgo la stringa col nome
    char *indice = strchr(aliasMsg, '!');
    int lgNome = strlen(aliasMsg) - strlen(indice) + 1;
    char *nome = (char*) malloc(lgNome * (sizeof (char)));
    memcpy(nome, aliasMsg, lgNome - 1);
    nome[lgNome - 1] = '\0';
    x->nomeClient = nome;

    //Raccolgo codice client e punti iniziali
    indice++;
    char *check = NULL;
    x->clientID = strtol(indice, &check, 10);
    indice = check + 1;
    check = NULL;
    x->clientPunti = strtol(indice, &check, 10);

    if (check != NULL) {
        if (*check != '\0');
        return false;
    }
    return true;

}

/**
 * 
 * @return 
 */
bool decBroadAggPunti(messaggio *x) {

    char * aliasMsg = x->msg + 1;

    //Raccolgo codice client e punti
    char *check = NULL;
    x->clientID = strtol(aliasMsg, &check, 10);
    aliasMsg = check + 1;
    check = NULL;
    x->clientPunti = strtol(aliasMsg, &check, 10);

    if (check != NULL) {
        if (*check != '\0')
            return false;
    }
    return true;
}

/**
 * 
 * @return 
 */
bool decInvDomanda(messaggio *x) {
    printf("%s\n", x->msg);

    char * aliasMsg = x->msg + 1;

    //Raccolgo codice client e punti
    char *check = NULL;
    x->domandaNum1 = strtol(aliasMsg, &check, 10);
    aliasMsg = check + 1;
    check = NULL;
    x->domandaNum2 = strtol(aliasMsg, &check, 10);

    if (check != NULL) {
        if (*check != '\0')
            return false;
    }
    printf("Finale %s %i %i\n", x->msg, x->domandaNum1, x->domandaNum2);
    return true;

}

messaggio leggiComm(messaggio msg, char* input) {

    char *indice = NULL, *indiceTmp = NULL;
    char tmp[20];

    printf("%s\n", input);

    //PID
    memcpy(tmp, input, 6);
    int pid;
    tmp[5] = '\0';
    sscanf(tmp, "%i", &pid);
    msg.pidMit = pid;
    indice = &input[6];

    //TIMESTAMP
    indiceTmp = strchr(indice, '!');
    memcpy(tmp, indice, (int) (indiceTmp - indice));
    tmp[17] = '\0';
    msg.timestring = (char*) malloc(17 * sizeof (char));
    strcpy(msg.timestring, tmp);


    //CODICE MSG
    indice = indiceTmp + 1;
    char *pp = NULL;
    msg.codiceMsg = strtol(indice, &pp, 10);


    //Copio la rimanente stringa in una più piccola e dealloco la grande e la sostituisco

    char *strShort = (char*) malloc((strlen(pp) + 1)*(sizeof (char)));
    memcpy(strShort, pp, strlen(pp));
    strShort[ strlen(pp)] = '\0';
    char* cancellando = msg.msg;
    msg.msg = strShort;
    free((cancellando));

    switch (msg.codiceMsg) {
        case 1: decInvDatiRisp(&msg);
            break;
        case 2: decRichPartec(&msg);
            break;
        case 3: //InvLogOut;
            break;
        case 4: //MesgCorrotto;
            break;
        case 5: //AccettaClient;
            break;
        case 6: //RifiutaClient;
            break;
        case 7: //InvClassifica;
            break;
        case 8: decBroadNuovoGiocatore(&msg);
            break;
        case 9: decBroadAggPunti(&msg);
            break;
        case 10: decInvDomanda(&msg);
            break;
        default: break;
    }
}
