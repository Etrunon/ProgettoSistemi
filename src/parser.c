#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

/**
 * Funzione che concatena le due stringhe in inpud secondo il formato definito dal protocollo del messaggio
 * @param messaggio stringa del messaggio parziale da concatenare
 * @param seconda stringa da aggiungere
 * @return true se success, false se errore
 */
bool concatena(char* messaggio, char* seconda) {

    sprintf(temp, "%s\0%s\0", prima, seconda);

    sprintf(messaggio, "%s", temp);

}

/**
 * Funzione che dato una STRINGA di 19 celle ritorna il timestamp come stringa seguito da \0
 * @return 
 */
char* timestamp(char* s) {

    time_t t = time(NULL);
    struct tm * p = localtime(&t);
    strftime(s, 19, "%A,%B,%d,%Y", p);

    printf("[%s]\n", s);
    printf("%i", (strlen(s) + 1));

    return s;
}

char* crInvDatiRisp(int pid, int rispo) {

}
char* crRichPartec(int pid);
char* crInvLogOut(int pid);
char* crMesgCorrotto(int pid);

char* crAccettaClient(int pid) {

}
char* crRifiutaClient(int pid);
char* crInvClassifica(int pid);
char* crBroadNuovoGiocatore(int pid);
char* crBroadAggPunti(int pid);
char* crInvDomanda(int pid);

