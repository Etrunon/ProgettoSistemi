#ifndef CONST_H
#define	CONST_H


extern int testing;

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_VIOLET "\x1b[35;1m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define ANSI_COLOR_BOLD_GREEN "\x1b[1;32;44m"
#define ANSI_COLOR_BOLD_CYAN "\x1b[1;36;40m"
#define SERVERPATH "/tmp/serverFIFO"
#define CLIENTFIFO "/tmp/gameClient"
#define MAX_FIFONAME 30
#define MSG_SIZE 80
#define MAXLINE 50
#define MAXCOMMAND 14
#define MAXNAME 14
#define MAXANSWER 5
#define NUM_CLIENT 10
#define WAIT_TIME 0.1


#define LARGHEZZASCHERMO 80

#define ALTEZZAPUNTI 12
#define BUFFERMESSAGGI 50
#define MESSAGGI_A_SCHERMO 5
#define BUFFMESSAGGIO 400
#endif	/* CONST_H */

