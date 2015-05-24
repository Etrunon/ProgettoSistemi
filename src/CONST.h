#ifndef CONST_H
#define	CONST_H

/*
 * Progetto: Multiplayer Game
 * A.A 2014/2015
 * Carlo Mion   165878
 * Luca Bosotti 164403
 */

extern int testing;
extern int seed;

/**
 Libreria contentente solamente costanti varie
 */

//Costanti di colorazione dell'interfaccia
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_VIOLET "\x1b[35;1m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_BOLD_GREEN "\x1b[1;32;44m"
#define ANSI_COLOR_BOLD_CYAN "\x1b[1;36;40m"

//Path usati per le fifo
#define SERVERPATH "/tmp/serverFIFO"
#define CLIENTFIFO "/tmp/gameClient"

//Costanti per le dimensioni dei vari messaggi
#define MAX_FIFONAME 30
#define MSG_SIZE 80
#define MAXLINE 50
#define MAXCOMMAND 14
#define MAXNAME 14
#define MAXANSWER 5
#define NUM_CLIENT 10

//Costanti usate per l'interfaccia del terminale
#define ALTEZZAPUNTI 12
#define BUFFERMESSAGGI 75
#define MESSAGGI_A_SCHERMO 5
#define BUFFMESSAGGIO 400
#endif	/* CONST_H */

