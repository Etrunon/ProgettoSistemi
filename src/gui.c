/*
 * Progetto: Multiplayer Game
 * A.A 2014/2015
 * Carlo Mion   165878
 * Luca Bosotti 164403
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "logica.h"
#include "gui.h"
#include "CONST.h"
#include "commands.h"
#include "client.h"
#include "guiMessages.h"

/*Variabile che indica la modalità di visualizzazione del terminale*/
GUIMode modalitaGUI = INIT;

/*Variabile usata per stampare correttamente in centro alcuni messaggi, nel caso in cui il terminale cambi di dimensione*/
int larghezzaSchermo = 80;

/**
 * Funzione che forka il programma e gli fa eseguire il comando di terminale "tput" con parametro "cols",
 * che ritorna la larghezza del terminale sul quale è usato.
 * Utilizzata per rendere l'interfaccia ridimensionabile in caso di resize del terminale
 */
void calcolaLarghezzaSchermo(int arg) {
    int pid, status;
    char buff [20];
    int fd[2];

    char* commands[] = {"tput", "cols", NULL};
    /*Creo PIPE per leggere l'output di tput*/
    pipe(fd);

    pid = fork();

    if (pid == 0) {
        /*Sposto l'output di tput perché scriva sulla PIPE*/
        dup2(fd[1], 1);
        close(fd[0]);
        /*Invoco tput con i parametri specificati*/
        execvp("tput", commands);
    } else {

        /*Attende che tput termini*/
        wait(&status);
        /*Leggo dalla PIPE il valore tornato da tput*/
        read(fd[0], buff, 20);
        larghezzaSchermo = strtol(buff, NULL, 10);
    }

    updateScreen();
    /*Mi imposto come signal handler nel caso il terinale cambi di nuovo dimensione*/
    signal(SIGWINCH, calcolaLarghezzaSchermo);
}

/*Funzione che usa "tput reset" per cancellare il contenuto dello schermo, e poter aggiornare il terminale con nuove informazioni*/
void clearScreen() {

    int pid = fork();
    if (pid == 0) {
        /*Eseguo il comado tput*/
        execlp("tput", "tput", "reset", (char*) NULL);
    } else {
        /*Attendo che tput termini*/
        wait(NULL);
    }

}

/*Stampa la linea di separazione nell'interfaccia grafica*/
void HorizontalLine(char* linea) {
    int i;
    char line [800] = {};
    for (i = 0; i < larghezzaSchermo; i++) {
        strcat(line, linea);
    }
    /*Stampa in maniera centrata la linea*/
    printf("%*s\n", larghezzaSchermo / 2 + (int) strlen(line) / 2, line);
}


//Stampare lo storico dei giocatori che hanno partecipato alla partita

void stampaStorico() {
    char line [BUFFMESSAGGIO] = {};
    /*indica lo spazio usato per ogni colonna*/
    int spazio = larghezzaSchermo / 3;
    /*Spazio che rimande per riempire la colonna, dopo aver stampato il contenuto
     Usato per centrare il contenuto delle colonne*/
    int spazioRimanente;

#ifndef DEBUGSTORICO
    /*Nel caso non ci siano giocatori nello storico*/
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

/*Calcola l'altezza della colonna dei punti da stampare nel grafico*/
int puntoNormalizzato(int punto) {
    int punteggio = (ALTEZZAPUNTI * punto) / maxWin;
    return punteggio;
}

/*Stampa il grafico dei punteggi dei giocatori*/
void playersGraph() {
    if (currentClients == 0) {
        /*Non ho giocatori, non stampo classifica*/
        return;
    }
    /*Indica l'ultimo punto stampato a schermo, mentre costruisco le colonne*/
    int ultimoPuntoStampato = ALTEZZAPUNTI;
    int i;
    /*Stringa usata per stampare correttamente le colonne dei punti*/
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


    /*Stampa le prime tre lettere del nome*/
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

/*Informazioni di gioco mostrate al server, come il numero di giocatori, il punteggio per la vittoria, etc.*/
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

/*Funzione richiamata dall'esterno per aggiornare lo schermo
 Utilizza la modalità GUI impostata precedentemente per scegliere quali parti dell'interfaccia mostrare*/
void updateScreen() {
    if (!testing) {
        /*Se sono in testing, non aggiungere handler per ridimensione finestra*/
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
        case VISUALIZZA_STORICO_SERVER:
        {
            clearScreen();
            header();
            infoServer();
            stampaStorico();
            printf("\r%s", "Q PER TORNARE INDIETRO:");

        }
            break;
        case EXIT_STORICO:
        {
            stampaStorico();
        }

    }
    fflush(stdout);
}

/*Wrapper per settare la modalità di visualizzazione della GUI*/
void SetGUIMode(GUIMode mode) {
    /*Se sono in testing, non cambiare modalità di visualizzazione*/
    if (modalitaGUI == TESTING_CLIENT || modalitaGUI == TESTING_SERVER || modalitaGUI == EXIT_STORICO) {
        /*a meno che non stia mostrando la classifica di fine partita*/
        if (mode == EXIT_STORICO) {
            modalitaGUI = EXIT_STORICO;
        }
    } else {
        modalitaGUI = mode;
    }
}