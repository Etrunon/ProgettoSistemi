/*
 * Progetto: Multiplayer Game
 * A.A 2014/2015
 * Carlo Mion   165878
 * Luca Bosotti 164403
 */


#ifndef GUI_H
#define	GUI_H

#include <stdbool.h>

/*Variabile globale usata anche da GUIMessages per centrare i messaggi*/
extern int larghezzaSchermo;

typedef enum GUIMode {
    INIT, //Valore iniziale
    TESTING_CLIENT, //client in testing, non mostrare alcun output
    TESTING_SERVER, //server in testing, mostra solto i messaggi di gioco
    STANDARD_CLIENT, //Client mentre gioca
    STANDARD_SERVER, //Server mentre gioca
    LOGIN_CLIENT, //Client prima di entrare in gioco
    EXIT_CLIENT, //Client dopo essere uscito dal gioco
    EXIT_SERVER, //Server dopo aver terminato la sua attività
    LOG, //Mostra lo storico di messaggi ricevuti
    VISUALIZZA_STORICO_SERVER, //Stampa lo storico dei giocatori
    EXIT_STORICO //classifica di fine gioco, usata quando il server
} GUIMode;

GUIMode modalitaGUI;

/*Permette di cambiare la modalità di visualizzazione della GUI*/
void SetGUIMode(GUIMode mode);

/*Aggiorna la schermata del terminale*/
void updateScreen();

void calcolaLarghezzaSchermo(int arg);

#endif	/* GUI_H */

