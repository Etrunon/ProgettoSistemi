/* 
 * File:   parser.h
 * Author: etrunon
 *
 * Created on May 8, 2015, 12:06 PM
 */

#ifndef PARSER_H
#define	PARSER_H


#include <time.h>

/**
 Struct contente tutti i dati logici dei messaggi inviati tramite le FIFO
 */
typedef struct messaggioTag {
    int pidMit;
    int numArg;
    struct tm timestamp;
    char messFinale[64];
} messaggio;

/**
 * Ciao
 * @return 
 */
char* crInvDatiRisp();

/**
 * 
 * @return 
 */
char* crRichPartec();

/**
 * 
 * @return 
 */
char* crInvLogOut();

/**
 * 
 * @return 
 */
char* crMesgCorrotto();

/**
 * 
 * @return 
 */
char* crAccettaClient();

/**
 * 
 * @return 
 */
char* crRifiutaClient();

/**
 * 
 * @return 
 */
char* crInvClassifica();

/**
 * 
 * @return 
 */
char* crBroadNuovoGiocatore();

/**
 * 
 * @return 
 */
char* crBroadAggPunti();

/**
 * 
 * @return 
 */
char* crInvDomanda();

/**
 * Funzione che dato l'array dei processi conosciuti con cui si è in contatto controlla se il mittente è nella lista.
 * @param contatti
 * @return 
 */
bool checkPid(int* contatti);


messaggio leggiComm(messaggio msg, char* input);


#endif	/* PARSER_H */

