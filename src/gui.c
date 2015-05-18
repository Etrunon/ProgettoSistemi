
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

void clearScreen() {
    int i;
    for (i = 0; i < 50; i++) {
        printf("\n");
    }
}

void HorizontalLine() {
    int i;
    char line [LARGHEZZASCHERMO * 7] = {};
    for (i = 0; i < LARGHEZZASCHERMO * 2; i++) {
        strcat(line, "\u2501");
    }

    printf("%*s\n", LARGHEZZASCHERMO + (int) strlen(line) / 2, line);
}

void print_image(FILE *fptr) {
    char read_string[128];
    while (fgets(read_string, sizeof (read_string), fptr) != NULL)
        printf("%s", read_string);
    printf("\n");
}

void header() {

    char *filename = "src/art.txt";
    FILE *fptr = NULL;

    if ((fptr = fopen(filename, "r")) != NULL) {
        //fprintf(stderr, "error opening %s\n", filename);
        print_image(fptr);
        fclose(fptr);
    }
    //char title [200] = ANSI_COLOR_BLUE "MULTIPLAYER GAME" ANSI_COLOR_RESET;
    //printf("%*s%*s\n", LARGHEZZASCHERMO + (int) strlen(title) / 2, title, LARGHEZZASCHERMO - (int) strlen(title) / 2, "");
    //strcpy(title, ANSI_COLOR_BLUE "GuessTheNumber GAME!" ANSI_COLOR_RESET);
    //printf("%*s%*s\n", LARGHEZZASCHERMO + (int) strlen(title) / 2, title, LARGHEZZASCHERMO - (int) strlen(title) / 2, "");
    HorizontalLine();
}

int puntoNormalizzato(int punto) {
    /*
        if (punto == 1)
            return 1;
     */
    int punteggio = (ALTEZZAPUNTI * punto) / maxWin;
    return punteggio;
}

void punteggiGiocatori() {
    int ultimoPuntoStampato = ALTEZZAPUNTI;
    int i;
    char format [5];
    int larghezzaAutomatica = 17;
    if (currentClients < 5)
        larghezzaAutomatica = 20;
    if (currentClients < 3)
        larghezzaAutomatica = 25;

    sprintf(format, "%c%i%c", '%', larghezzaAutomatica - currentClients, 's');
    for (ultimoPuntoStampato; ultimoPuntoStampato > 0; ultimoPuntoStampato--) {
        printf(ANSI_COLOR_BLUE);
        if (ultimoPuntoStampato > ALTEZZAPUNTI / 4)
            printf(ANSI_COLOR_GREEN);
        if (ultimoPuntoStampato > (ALTEZZAPUNTI / 4) * 2)
            printf(ANSI_COLOR_YELLOW);
        if (ultimoPuntoStampato > (ALTEZZAPUNTI / 4) * 3)
            printf(ANSI_COLOR_RED);
        for (i = 0; i < currentClients; i++) {
            int puntoNorm = puntoNormalizzato(giocatoriCorrenti[i].punteggio);
            if (ultimoPuntoStampato <= puntoNorm) {
                printf(format, "#");
            }
        }
        printf(ANSI_COLOR_RESET);
        printf("\n");
    }

    sprintf(format, "%c%i%c", '%', larghezzaAutomatica - currentClients, 'i');
    for (i = 0; i < currentClients; i++) {
        printf(format, i + 1);
    }
    printf("\n");


    /*TODO: Stampa punteggio e nome*/
    sprintf(format, "%c%i%c", '%', -(larghezzaAutomatica - currentClients) + 2, 's');
    printf(format, "");
    sprintf(format, "%c%i%c", '%', -(larghezzaAutomatica - currentClients), 's');
    for (i = 0; i < currentClients; i++) {
        printf(format, "aaa");
    }
    printf("\n");
}

void players() {
    if (currentClients > 0) {
        punteggiGiocatori();
        HorizontalLine();
    }
}

void printDomanda() {
    char line [200];
    sprintf(line, "%s%s%i%s%s%s%i%s%s", "Quanto fa ",
            ANSI_COLOR_MAGENTA, domandaCorrente.numero1, ANSI_COLOR_RESET,
            " + ",
            ANSI_COLOR_MAGENTA, domandaCorrente.numero2, ANSI_COLOR_RESET,
            "?");
    printf("%*s\n", LARGHEZZASCHERMO + (int) strlen(line) / 2 + 5, line);
}

void messagges(int number) {
    stampaMessaggi(number);
    HorizontalLine();
}

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
            break;
        }
        case STANDARD_CLIENT:
        {
            players();
            messagges(MESSAGGI_A_SCHERMO);
            printDomanda();
            printf("\r%s", "Client:");
            break;
        }
        case EXIT_CLIENT:
        {
            messagges(MESSAGGI_A_SCHERMO);
            break;
        }
        case STANDARD_SERVER:
        {
            players();
            infoServer();
            messagges(MESSAGGI_A_SCHERMO);
            printDomanda();
            printf("\r%s", "Server:");
            break;
        }
        case EXIT_SERVER:
        {
            players();
            infoServer();
            messagges(MESSAGGI_A_SCHERMO);
            break;
        }
        case LOG:
        {
            messagges(BUFFERMESSAGGI);
            printf("\r%s", "Q PER TORNARE INDIETRO:");
            break;
        }
    }
    fflush(stdout);
}

void SetGUIMode(GUIMode mode) {
    modalitaGUI = mode;
}