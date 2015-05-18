#define _GNU_SOURCE
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
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

GUIMode modalitaGUI = INIT;

void clearScreen() {
    /*
        int i;
        for (i = 0; i < 50; i++) {
            printf("\n");
        }
     */
    //printf("%c[2J\n", 27);
    int pid = fork();
    if (pid == 0) {
        execlp("tput", "tput", "reset", (char*) NULL);
    } else {
        wait(NULL);
    }
    //printf("ciao\n");
}

/*Stampa la linea di separazione nell'interfaccia grafica*/
void HorizontalLine() {
    int i;
    char line [LARGHEZZASCHERMO * 7] = {};
    for (i = 0; i < LARGHEZZASCHERMO; i++) {
        strcat(line, "\u2501");
    }

    printf("%*s\n", LARGHEZZASCHERMO / 2 + (int) strlen(line) / 2, line);
}

/*Stampa il logo del gioco*/
void header() {

    char *filename = "/tmp/art.txt";
    FILE *fptr = NULL;

    if ((fptr = fopen(filename, "r")) != NULL) {
        /*Se riesce ad aprire il file con il logo*/
        char* read_string = (char*) malloc(sizeof (char)*250);
        size_t size = sizeof (char) * 250;
        int stampati, letti;
        while ((letti = getline(&read_string, &size, fptr)) != -1) {
            read_string[letti - 1 ] = '\0';
            stampati = printf("%*s\n", LARGHEZZASCHERMO / 2 + (int) strlen(read_string) / 2, read_string);

        }
        //printf("\n");
        free(read_string);

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
        int i = 0;
        /*
                for (int i = 0; i < ALTEZZAPUNTI; i++) {
                    printf("\n");
                }
         */
        /*Non ho giocatori, non stampo classifica*/
        return;
    }
    int ultimoPuntoStampato = ALTEZZAPUNTI;
    int i;
    char format [5];

    /* Vecchio metodo per calcolare larghezza grafico in base a num giocatori
        int larghezzaAutomatica = 17;
        if (currentClients < 5)
            larghezzaAutomatica = 20;
        if (currentClients < 3)
            larghezzaAutomatica = 25;
     */

    /*Calcola dinamicamente lo spazio per centrare la classifica in base al numero di giocatori*/
    sprintf(format, "%c%i%c", '%', LARGHEZZASCHERMO / (currentClients + 1), 's');

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
    sprintf(format, "%c%i%c", '%', LARGHEZZASCHERMO / (currentClients + 1), 'i');
    for (i = 0; i < currentClients; i++) {
        printf(format, getPuntiGiocatore(giocatoriCorrenti[i]->IDGiocatore));
    }
    printf("\n");


    /*TODO: Stampa punteggio e nome*/
    sprintf(format, "%c%i%c", '%', -(LARGHEZZASCHERMO / (currentClients + 1)) + 2, 's');
    printf(format, "");
    sprintf(format, "%c%i%c", '%', -(LARGHEZZASCHERMO / (currentClients + 1)), 's');
    for (i = 0; i < currentClients; i++) {
        char tmp [MAXNAME], nomeAbbreviato [4];
        getNomeGiocatore(giocatoriCorrenti[i]->IDGiocatore, tmp);

        memccpy(&nomeAbbreviato, tmp, 0, 3);
        nomeAbbreviato[3] = '\0';
        printf(format, nomeAbbreviato);
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

    printf("%*s", LARGHEZZASCHERMO / 2 + ((int) strlen(line) / 2) + 5, line);
    printf("\n");
}

/*Stampa la lista dei messaggi più recenti,
 * tanti quanti il paramentro number*/
void messagges(int number) {

    stampaMessaggi(number);
    //HorizontalLine();
}

/*Informazioni di gioco mostrate sul server*/
void infoServer() {

    char informazione [BUFFMESSAGGIO];
    int spazioRimanente;
    int spazio = (LARGHEZZASCHERMO / 2) - 2;

    strcpy(informazione, "Max giocatori:");
    spazioRimanente = spazio - strlen(informazione);

    printf("%s%s%*i%s", informazione, ANSI_COLOR_YELLOW, spazioRimanente, maxClients, ANSI_COLOR_RESET);
    printf(" \u2503 ");

    strcpy(informazione, "Giocatori connessi:");
    spazioRimanente = spazio - strlen(informazione);
    printf("%s%s%*i%s\n", informazione, ANSI_COLOR_YELLOW, spazioRimanente, currentClients, ANSI_COLOR_RESET);

    strcpy(informazione, "Punteggio per vittoria:");
    spazioRimanente = spazio - strlen(informazione);
    printf("%s%s%*i%s", informazione, ANSI_COLOR_YELLOW, spazioRimanente, maxWin, ANSI_COLOR_RESET);
    printf(" \u2503 ");

    printf("\n");
    HorizontalLine();

}

void updateScreen() {
    switch (modalitaGUI) {
        case LOGIN_CLIENT:
        {
            clearScreen();
            header();
            messagges(MESSAGGI_A_SCHERMO);
            printf("\r%s", "Inserisci il nome:");
        }
            break;
        case STANDARD_CLIENT:
        {
            clearScreen();
            header();
            players();
            messagges(MESSAGGI_A_SCHERMO);
            printDomanda();
            printf("\r%s", "Client:");
        }
            break;
        case TESTING_CLIENT:
        {
            break;
        case EXIT_CLIENT:
        {
            clearScreen();
            header();
            messagges(MESSAGGI_A_SCHERMO);
        }
            break;
        case STANDARD_SERVER:
        {
            clearScreen();
            header();
            players();
            infoServer();
            messagges(MESSAGGI_A_SCHERMO);
            printDomanda();
            printf("\r%s", "Server:");
        }
            break;
        case TESTING_SERVER:
        {
            clearScreen();
            messagges(BUFFERMESSAGGI);
        }
            break;    
        case EXIT_SERVER:
        {
            clearScreen();
            header();
            players();
            infoServer();
            messagges(MESSAGGI_A_SCHERMO);
        }
            break;
        case LOG:
        {
            clearScreen();
            messagges(BUFFERMESSAGGI);
            printf("\r%s", "Q PER TORNARE INDIETRO:");
        }
            break;
    }
    fflush(stdout);
}

/*Wrapper per settare la modalità di visualizzazione della GUI*/
void SetGUIMode(GUIMode mode) {
    if (modalitaGUI != TESTING_CLIENT && modalitaGUI != TESTING_SERVER)
        modalitaGUI = mode;
}