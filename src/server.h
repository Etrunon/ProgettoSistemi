/*
 * File:   server.h
 * Author: mion00
 *
 * Created on May 7, 2015, 5:17 PM
 */

#ifndef SERVER_H
#define	SERVER_H

#include <stdbool.h>

/*Controlla che il server sia unico
 * Se vi è già un server, abortisce il programma
 * In caso non vi sia un altro server, prepara i mezzi di comunicazione con i client
 */
int initServer(int maxClients, int maxWin);

#endif	/* SERVER_H */

