/*
 * File:   gui.h
 * Author: mion00
 *
 * Created on May 14, 2015, 9:51 PM
 */

#ifndef GUI_H
#define	GUI_H

#include <stdbool.h>

typedef enum GUIMode {
    STANDARD_CLIENT, //Client mentre gioca
    STANDARD_SERVER, //Server mentre gioca
    LOGIN_CLIENT, //Client prima di entrare in gioco
    EXIT_CLIENT, //Client dopo essere uscito dal gioco
    EXIT_SERVER, //Server dopo aver terminato la sua attività
    LOG //Mostra lo storico di messaggi ricevuti
} GUIMode;

GUIMode modalitaGUI;

/*/*Permette di cambiare la modalità di visualizzazione della GUI*/
void SetGUIMode(GUIMode mode);

/*Aggiorna la schermata del terminale*/
void updateScreen();

#endif	/* GUI_H */

