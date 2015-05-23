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

int larghezzaSchermo = 80;

/**
 * Funzione che forka il programma e gli fa eseguire il comando di terminale "tput" che ritorna la larghezza 
 * del terminale sul quale è usato. 
 * Utilizzata per rendere l'interfaccia ridimensionabile
 * @param arg
 */
void calcolaLarghezzaSchermo(int arg) {
    int pid, status;
    char buff [20];
    int fd[2];

    char* commands[] = {"tput", "cols", NULL};
    pipe(fd);

    pid = fork();

    if (pid == 0) {
        dup2(fd[1], 1);
        close(fd[0]);
        execvp("tput", commands);
    } else {

        wait(&status);
        read(fd[0], buff, 20);
        larghezzaSchermo = strtol(buff, NULL, 10);
        //printf("\nLarghezza schermo: %i\n", number);
    }

    updateScreen();

    signal(SIGWINCH, calcolaLarghezzaSchermo);
}

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
void HorizontalLine(char* linea) {
    int i;
    char line [800] = {};
    for (i = 0; i < larghezzaSchermo; i++) {

        strcat(line, linea);
    }

    printf("%*s\n", larghezzaSchermo / 2 + (int) strlen(line) / 2, line);
}

void horizontalSeparator(int spazio) {
    char line [50] = {};
    sprintf(line, "\u2502");
    printf("%*s", spazio / 2 + (int) strlen(line) / 2, line);
}

/**
 * Modalità d'interfaccia che consente di stampare lo storico di gioco
 */
void stampaStorico() {
    char line [BUFFMESSAGGIO] = {};
    int spazio = larghezzaSchermo / 3;
    int spazioRimanente;

#ifndef DEBUGSTORICO
    if (indiceStorico == 0) {
        sprintf(line, "Nessun dato presente nello storico\n");
        printf("%*s\n", larghezzaSchermo / 2 + (int) strlen(line) / 2, line);
        return;
    }
#endif

    //Stampo l'intestazione dello storico
    printf(ANSI_COLOR_BLUE);
    sprintf(line, "Tempo di uscita");
    printf("\r%*s", spazio / 2 + (int) strlen(line) / 2, line);
    spazioRimanente = spazio - (spazio / 2 + (int) strlen(line) / 2);
    printf("%*s", spazioRimanente, "");

    sprintf(line, "Nome");
    printf("%*s", spazio / 2 + (int) strlen(line) / 2, line);
    spazioRimanente = spazio - (spazio / 2 + (int) strlen(line) / 2);
    printf("%*s", spazioRimanente, "");

    sprintf(line, "ID");
    spazio = larghezzaSchermo / 8;
    printf("%*s", spazio / 2 + (int) strlen(line) / 2, line);
    spazioRimanente = spazio - (spazio / 2 + (int) strlen(line) / 2);
    printf("%*s", spazioRimanente, "");

    sprintf(line, "Punti");
    printf("%*s", spazio / 2 + (int) strlen(line) / 2, line);
    spazioRimanente = spazio - (spazio / 2 + (int) strlen(line) / 2);
    printf("%*s", spazioRimanente, "");

    printf("\n"ANSI_COLOR_RESET);


    //Stampo tutti i giocatori nello storico
    int i = 0;
    for (i; i < indiceStorico; i++) {
        spazio = larghezzaSchermo / 3;
        sprintf(line, "%s", storico[i]->time);
        printf("%*s", spazio / 2 + (int) strlen(line) / 2, line);
        spazioRimanente = spazio - (spazio / 2 + (int) strlen(line) / 2) + 2;
        printf("%*s", spazioRimanente, "\u2502");

        sprintf(line, "%s", storico[i]->g->name);
        printf("%*s", spazio / 2 + (int) strlen(line) / 2, line);
        spazioRimanente = spazio - (spazio / 2 + (int) strlen(line) / 2) + 2;
        printf("%*s", spazioRimanente, "\u2502");

        spazio = larghezzaSchermo / 8;
        sprintf(line, "%02i", storico[i]->g->IDGiocatore);
        printf("%*s", spazio / 2 + (int) strlen(line) / 2, line);
        spazioRimanente = spazio - (spazio / 2 + (int) strlen(line) / 2) + 2;
        printf("%*s", spazioRimanente, "\u2502");

        sprintf(line, "%i", storico[i]->g->punteggio);
        printf("%*s", spazio / 2 + (int) strlen(line) / 2, line);
        printf("\n");
    }
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
            stampati = printf("%*s\n", larghezzaSchermo / 2 + (int) strlen(read_string) / 2, read_string);

        }

        free(read_string);
        fclose(fptr);
        HorizontalLine("\u2500");
    }
}

/*Calcola l'altezza della colonna dei punti da stampare*/
int puntoNormalizzato(int punto) {

    int punteggio = (ALTEZZAPUNTI * punto) / maxWin;
    return punteggio;
}

/*Stampa la classifica dei giocatori*/
void playersGraph() {
    if (currentClients == 0) {
        int i = 0;
        /*Non ho giocatori, non stampo classifica*/
        return;
    }
    int ultimoPuntoStampato = ALTEZZAPUNTI;
    int i;
    char format [20];

    /*Calcola dinamicamente lo spazio per centrare la classifica in base al numero di giocatori*/
    sprintf(format, "%c%i%c", '%', larghezzaSchermo / (currentClients + 1), 's');

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
    sprintf(format, "%c%i%c", '%', larghezzaSchermo / (currentClients + 1), 'i');
    for (i = 0; i < currentClients; i++) {
        printf(format, getPuntiGiocatore(giocatoriCorrenti[i]->IDGiocatore));
    }
    printf("\n");


    /*Stampa punteggio e nome*/
    sprintf(format, "%c%i%c", '%', -(larghezzaSchermo / (currentClients + 1)) + 2, 's');
    printf(format, "");
    sprintf(format, "%c%i%c", '%', -(larghezzaSchermo / (currentClients + 1)), 's');
    for (i = 0; i < currentClients; i++) {

        char tmp [MAXNAME], nomeAbbreviato [4];
        getNomeGiocatore(giocatoriCorrenti[i]->IDGiocatore, tmp);

        memccpy(&nomeAbbreviato, tmp, 0, 3);
        nomeAbbreviato[3] = '\0';
        printf(format, nomeAbbreviato);
    }
    printf("\n");
    HorizontalLine("\u2501");
}

/*Stampa la domanda*/
void printDomanda() {

    char line [200];
    sprintf(line, "%s%s%i%s%s%s%i%s%s", "Quanto fa ",
            ANSI_COLOR_VIOLET, domandaCorrente.numero1, ANSI_COLOR_RESET,
            " + ",
            ANSI_COLOR_VIOLET, domandaCorrente.numero2, ANSI_COLOR_RESET,
            "?");

    printf("%*s", larghezzaSchermo / 2 + ((int) strlen(line) / 2) + 5, line);
    printf("\n");
}

/*Stampa la lista dei messaggi più recenti,
 * tanti quanti il paramentro number*/
void messagges(int number) {

    stampaMessaggi(number);
}

/*Informazioni di gioco mostrate sul server*/
void infoServer() {

    char informazione [BUFFMESSAGGIO];
    int spazioRimanente;
    int spazio = (larghezzaSchermo / 2) - 2;

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
    HorizontalLine("\u2501");

}

void updateScreen() {
    if (!testing) {
        signal(SIGWINCH, calcolaLarghezzaSchermo);
    }
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
            playersGraph();
            messagges(MESSAGGI_A_SCHERMO);
            printDomanda();
            printf("\r%s", "Client:");
        }
            break;
        case TESTING_CLIENT:
        {

        }
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
            playersGraph();
            infoServer();
            messagges(MESSAGGI_A_SCHERMO);
            printDomanda();
            printf("\r%s", "Server:");
        }
            break;
        case TESTING_SERVER:
        {
            messagges(1);
        }
            break;
        case EXIT_SERVER:
        {
            clearScreen();
            header();
            infoServer();
            stampaStorico();
            HorizontalLine("\u2501");
            messagges(MESSAGGI_A_SCHERMO);
        }
            break;
        case LOG:
        {

            clearScreen();
            messagges(BUFFERMESSAGGI);
            HorizontalLine("\u2501");
            printf("\r%s", "Q PER TORNARE INDIETRO:");
        }
            break;
        case VISUALIZZA_CLASSIFICA_SERVER:
        {
            clearScreen();
            header();
            infoServer();
            stampaStorico();
            printf("\r%s", "Q PER TORNARE INDIETRO:");

        }
            break;
        case EXIT_CLASSIFICA:
        {
            stampaStorico();
        }

    }
    fflush(stdout);
}

/*Wrapper per settare la modalità di visualizzazione della GUI*/
void SetGUIMode(GUIMode mode) {
    if (modalitaGUI == TESTING_CLIENT || modalitaGUI == TESTING_SERVER || modalitaGUI == EXIT_CLASSIFICA) {
        if (mode == EXIT_CLASSIFICA) {
            modalitaGUI = EXIT_CLASSIFICA;
        }
    } else {
        modalitaGUI = mode;
    }
}