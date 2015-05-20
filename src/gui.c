
#include <stdio.h>
#include <string.h>
#include "logica.h"
#include "gui.h"
#include "CONST.h"
#include "commands.h"
#include "client.h"
#include "guiMessages.h"
#include <wchar.h>
#include <unistd.h>

GUIMode modalitaGUI;

void clearScreen() {
    /*
        int i;
        for (i = 0; i < 50; i++) {
            printf("\n");
        }
     */
    printf("%c[2J\n", 27);
    //printf("ciao\n");
}

/*Stampa la linea di separazione nell'interfaccia grafica*/
void HorizontalLine() {
    int i;
    char line [LARGHEZZASCHERMO * 7] = {};
    for (i = 0; i < LARGHEZZASCHERMO * 2; i++) {
        strcat(line, "\u2501");
    }

    printf("%*s\n", LARGHEZZASCHERMO + (int) strlen(line) / 2, line);
}

/*Stampa il logo del gioco*/
void header() {

    char *filename = "/tmp/art.txt";
    FILE *fptr = NULL;

    if ((fptr = fopen(filename, "r")) != NULL) {
        /*Se riesce ad aprire il file con il logo*/
        char read_string[128];
        while (fgets(read_string, sizeof (read_string), fptr) != NULL)
            printf("%s", read_string);
        printf("\n");
        fclose(fptr);
        HorizontalLine();
    }
}

/*Calcola l'altezza della colonna dei punti da stampare*/
int puntoNormalizzato(int punto) {
    /*
        if (punto == 1)
            return 1;
     */
    int punteggio = (ALTEZZAPUNTI * punto) / maxWin;
    return punteggio;
}

/*Stampa la classifica dei giocatori*/
void players() {
    if (currentClients == 0) {
        /*Non ho giocatori, non stampo classifica*/
        return;
    }
    int ultimoPuntoStampato = ALTEZZAPUNTI;
    int i;
    char format [5];
    int larghezzaAutomatica = 17;
    if (currentClients < 5)
        larghezzaAutomatica = 20;
    if (currentClients < 3)
        larghezzaAutomatica = 25;

    /*Calcola dinamicamente lo spazio per centrare la classifica in base al numero di giocatori*/
    sprintf(format, "%c%i%c", '%', larghezzaAutomatica - currentClients, 's');

    for (ultimoPuntoStampato; ultimoPuntoStampato > 0; ultimoPuntoStampato--) {
        /*Cicla stampando dal punteggio più alto al più basso*/
        printf(ANSI_COLOR_BLUE);
        if (ultimoPuntoStampato > ALTEZZAPUNTI / 4)
            printf(ANSI_COLOR_GREEN);
        if (ultimoPuntoStampato > (ALTEZZAPUNTI / 4) * 2)
            printf(ANSI_COLOR_YELLOW);
        if (ultimoPuntoStampato > (ALTEZZAPUNTI / 4) * 3)
            printf(ANSI_COLOR_RED);
        for (i = 0; i < currentClients; i++) {
            /*Controlla se il punteggio di ogni giocatore è tale che deve essere stampato nella riga attuale*/
            int puntoNorm = puntoNormalizzato(giocatoriCorrenti[i]->punteggio);
            if (ultimoPuntoStampato <= puntoNorm) {
                printf(format, "#");
            }
        }
        printf(ANSI_COLOR_RESET);
        printf("\n");
    }

    /*Stampa l'ID del giocatore*/
    sprintf(format, "%c%i%c", '%', larghezzaAutomatica - currentClients, 'i');
    for (i = 0; i < currentClients; i++) {
        printf(format, getPuntiGiocatore(giocatoriCorrenti[i]->IDGiocatore));
    }
    printf("\n");


    /*TODO: Stampa punteggio e nome*/
    sprintf(format, "%c%i%c", '%', -(larghezzaAutomatica - currentClients) + 2, 's');
    printf(format, "");
    sprintf(format, "%c%i%c", '%', -(larghezzaAutomatica - currentClients), 's');
    for (i = 0; i < currentClients; i++) {
        char tmp [MAXNAME];
        getNomeGiocatore(giocatoriCorrenti[i]->IDGiocatore, tmp);
        printf(format, tmp);
    }
    printf("\n");
    HorizontalLine();
}

/*Stampa la domanda*/
void printDomanda() {
    char line [200];
    sprintf(line, "%s%s%i%s%s%s%i%s%s", "Quanto fa ",
            ANSI_COLOR_MAGENTA, domandaCorrente.numero1, ANSI_COLOR_RESET,
            " + ",
            ANSI_COLOR_MAGENTA, domandaCorrente.numero2, ANSI_COLOR_RESET,
            "?");
    printf("%*s\n", LARGHEZZASCHERMO + (int) strlen(line) / 2 + 5, line);
}

/*Stampa la lista dei messaggi più recenti,
 * tanti quanti il paramentro number*/
void messagges(int number) {
    stampaMessaggi(number);
    HorizontalLine();
}

/*Informazioni di gioco mostrate sul server*/
void infoServer() {

    /*Messaggio di benvenuto*/
    printf("%-35s%s%-3i%s\u2503 %-35s%s%-3i%s\n%-35s%s%-3i%s\u2503\n",
            "Max giocatori:", ANSI_COLOR_YELLOW, maxClients, ANSI_COLOR_RESET,
            "Giocatori connessi:", ANSI_COLOR_YELLOW, currentClients, ANSI_COLOR_RESET,
            "Punteggio per vittoria:", ANSI_COLOR_YELLOW, maxWin, ANSI_COLOR_RESET);
    HorizontalLine();

}

void updateScreen() {
    clearScreen();
    header();
    switch (modalitaGUI) {
        case LOGIN_CLIENT:
        {
            messagges(MESSAGGI_A_SCHERMO);
            printf("\r%s", "Inserisci il nome:");
        }
            break;
        case STANDARD_CLIENT:
        {
            players();
            messagges(MESSAGGI_A_SCHERMO);
            printDomanda();
            printf("\r%s", "Client:");
        }
            break;
        case EXIT_CLIENT:
        {
            messagges(MESSAGGI_A_SCHERMO);
        }
            break;
        case STANDARD_SERVER:
        {
            players();
            infoServer();
            messagges(MESSAGGI_A_SCHERMO);
            printDomanda();
            printf("\r%s", "Server:");
        }
            break;
        case EXIT_SERVER:
        {
            players();
            infoServer();
            messagges(MESSAGGI_A_SCHERMO);
        }
            break;
        case LOG:
        {
            messagges(BUFFERMESSAGGI);
            printf("\r%s", "Q PER TORNARE INDIETRO:");
        }
            break;
    }
    fflush(stdout);
}

/*Wrapper per settare la modalità di visualizzazione della GUI*/
void SetGUIMode(GUIMode mode) {
    modalitaGUI = mode;
}