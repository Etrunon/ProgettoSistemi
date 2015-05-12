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
    msg.clientSpecificato = -1;
    msg.clientSpecPunti = -1;
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

bool crInvClassifica(messaggio x, char* classifica) {
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

    memcpy(x->pathFifo, x->msg, lgPath);
    x->pathFifo[(lgPath + 1)] = '\0';

    /*
     TODO l'ultimo carattere è una schifezza!!
     */
    printf("%s\n%i\n%i\n", x->pathFifo, strlen(x->pathFifo), lgMax);

    return true;
}

/**
 * 
 * @return 
 */
bool decInvLogOut(messaggio *x) {

}

/**
 * 
 * @return 
 */
bool decMesgCorrotto(messaggio *x) {

}

/**
 * 
 * @return 
 */
bool decAccettaClient(messaggio *x) {

}

/**
 * 
 * @return 
 */
bool decRifiutaClient(messaggio *x) {

}

/**
 * 
 * @return 
 */
bool decInvClassifica(messaggio *x) {

}

/**
 * 
 * @return 
 */
bool decBroadNuovoGiocatore(messaggio *x) {

}

/**
 * 
 * @return 
 */
bool decBroadAggPunti(messaggio *x) {

}

/**
 * 
 * @return 
 */
bool decInvDomanda(messaggio *x) {

}

messaggio leggiComm(messaggio msg, char* input, int* contatti, int dim) {

    char *indice = NULL, *indiceTmp = NULL;
    char tmp[20];

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
        case 3: decInvLogOut(&msg);
            break;
        case 4: decMesgCorrotto(&msg);
            break;
        case 5: decAccettaClient(&msg);
            break;
        case 6: decRifiutaClient(&msg);
            break;
        case 7: decInvClassifica(&msg);
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
