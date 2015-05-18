/* 
 * File:   riparser.h
 * Author: etrunon
 *
 * Created on May 15, 2015, 12:53 PM
 */

#ifndef RIPARSER_H
#define	RIPARSER_H

typedef struct messaggioTag2 {
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
    bool corretta;

} messaggio2;

messaggio2* messaggio2Constructor();

void messaggio2Destructor(messaggio2 *x);

bool creaMessaggio2(messaggio2 *msg);

void traduciComm2(messaggio2 *msg);


#endif	/* RIPARSER_H */

