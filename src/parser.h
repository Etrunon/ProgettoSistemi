/* 
 * File:   parser.h
 * Author: etrunon
 *
 * Created on May 8, 2015, 12:06 PM
 */

#ifndef PARSER_H
#define	PARSER_H

#include <time.h>
#include "CONST.h"

/*
 * TODO:
 *  - Refactoring
 *  - Allocazione dimanica della string in invio
 *  - 
 */

/**
 Struct contente tutti i dati logici dei messaggi inviati tramite le FIFO
 */
typedef struct messaggioTag {
    int pidMit;
    int codiceMsg;
    char* timestring;
    char* msg;
    char* pathFifo;
    char* nomeClient;
    int numeroClient;
    int valRisposta;
    int clientID;
    int clientPunti;
    int domandaNum1;
    int domandaNum2;

} messaggio;

messaggio* messaggioConstructor();

void messaggioDestructor(messaggio *x);

bool creaMessaggio(messaggio *msg);

messaggio traduciComm(messaggio *msg);



#endif	/* PARSER_H */

