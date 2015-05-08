#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

/**
 * Funzione che concatena un nuovo segmento al messaggio finale. Se il risultato della concatenazione fosse piu lungo di
 * 64 (lunghezza massima della stringa in uscita sulla fifo) la funzione non concatenerebbe e ritornerebbe false.
 * @param messaggio stringa del messaggio parziale da concatenare
 * @param seconda stringa da aggiungere
 * @return true se success, false se errore
 */
bool concatena(char* msg, int indice, char* seconda) {

    if ((indice + strlen(seconda)) >= 64) {
        return false;
    }

    sprintf(msg, "%s!%s", msg, seconda);
    return true;
}

/**
 * Funzione che data una STRINGA di 19 celle ritorna il timestamp come stringa seguito da \0
 * @return 
 */
char* timestamp(char* s) {

    time_t t = time(NULL);
    struct tm * p = localtime(&t);
    strftime(s, 19, "%A,%B,%d,%Y", p);

    return s;
}

/*
 TODO Sistemare chiamata qui alla struct. Parser.c non conosce la dimensione della variabile messaggio->messFinale[] dichiarata
 *      nel file parser.h
 */
char* crInvDatiRisp(int rispo) {
    messaggio msg;

    msg->messFinale = "Ciao";

    printf("%s", msg->messFinale);
}
char* crRichPartec();
char* crInvLogOut();
char* crMesgCorrotto();

char* crAccettaClient() {

}
char* crRifiutaClient();
char* crInvClassifica();
char* crBroadNuovoGiocatore();
char* crBroadAggPunti();
char* crInvDomanda();

