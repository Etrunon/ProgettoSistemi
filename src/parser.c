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
    strftime(s, 25, "%y-%m-%d %H:%M:%S", p);

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

    sprintf(tmp, "!1!%i", rispo);
    strcat(x.msg, tmp);

    //sprintf(x.msg, "%s%s", x.msg, tmp);

    free(tmp);
    return true;
}

bool crRichPartec(messaggio x, char* pathFifo) {

    headerMsg(x);
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

    headerMsg(x);

    char* tmp = (char*) malloc(3 * sizeof (char));

    sprintf(tmp, "!3");
    strcat(x.msg, tmp);

    free(tmp);

};

bool crMesgCorrotto(messaggio x) {
    headerMsg(x);

    char* tmp = (char*) malloc(3 * sizeof (char));

    sprintf(tmp, "!4");
    strcat(x.msg, tmp);

    free(tmp);
};

bool crAccettaClient(messaggio x) {

    headerMsg(x);

    char* tmp = (char*) malloc(3 * sizeof (char));

    sprintf(tmp, "!5");
    strcat(x.msg, tmp);

    free(tmp);
}

bool crRifiutaClient(messaggio x) {
    headerMsg(x);

    char* tmp = (char*) malloc(3 * sizeof (char));

    sprintf(tmp, "!6");
    strcat(x.msg, tmp);

    free(tmp);
}

bool crInvClassifica(messaggio x, char* classifica) {
    headerMsg(x);

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

    headerMsg(x);

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

    headerMsg(x);

    char* tmp = (char*) malloc(12 * sizeof (char));

    sprintf(tmp, "!9!%i!%i", codice, punti);
    strcat(x.msg, tmp);

    free(tmp);
}

bool crInvDomanda(messaggio x, int primoNum, int secondoNum) {
    headerMsg(x);

    char* tmp = (char*) malloc(11 * sizeof (char));

    sprintf(tmp, "!10!%i!%i", primoNum, secondoNum);
    strcat(x.msg, tmp);

    free(tmp);
}

bool checkPid(int*contatti, int dim, int pid) {
    return true;
    return false;
}

messaggio leggiComm(messaggio msg, char* input, int* contatti, int dim) {

    char *indice = NULL, *indiceTmp = NULL;
    char tmp[20];
    memcpy(tmp, input, 6);

    //PID
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
    indiceTmp = strchr(indice, '!');

    printf("%s\n%s", indice, indiceTmp);

    char tmp2[20];
    memcpy(tmp2, indice, (int) (indiceTmp - indice));

    printf("\n Print di tmp2 : \n%s\n %lu \t %lu \n", tmp2, (long) indice, (long) indiceTmp);

    //sscanf(tmp, "%i", msg.codiceMsg);
    printf("%s \t %i", indice, msg.codiceMsg);




    printf("\n\nPrintfFinale\n%s\nOriginale: %s\nLong: %i\n", tmp, input, pid);
    //checkPid(contatti, );
}


