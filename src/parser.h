/* 
 * File:   parser.h
 * Author: etrunon
 *
 * Created on May 8, 2015, 12:06 PM
 */

#ifndef PARSER_H
#define	PARSER_H

#define MSG_SIZE 50
#include <time.h>

/**
 Struct contente tutti i dati logici dei messaggi inviati tramite le FIFO
 */
typedef struct messaggioTag {
    int pidMit;
    int codiceMsg;
    int numArg;
    struct tm timestamp;
    char* timestring;
    char* msg;
    int* dato;
    int* dato2;

} messaggio;

/**
 * Ciao
 * @return 
 */
bool crInvDatiRisp(messaggio x, int risposta);

/**
 * 
 * @return 
 */
bool crRichPartec(messaggio x, char* pathFifo);

/**
 * 
 * @return 
 */
bool crInvLogOut(messaggio x);

/**
 * 
 * @return 
 */
bool crMesgCorrotto(messaggio x);

/**
 * 
 * @return 
 */
bool crAccettaClient(messaggio x);

/**
 * 
 * @return 
 */
bool crRifiutaClient(messaggio x);

/**
 * 
 * @return 
 */
bool crInvClassifica(messaggio x, char* classifica);

/**
 * 
 * @return 
 */
bool crBroadNuovoGiocatore(messaggio x, char* nome, int punti, int codice);

/**
 * 
 * @return 
 */
bool crBroadAggPunti(messaggio x, int codice, int punti);

/**
 * 
 * @return 
 */
bool crInvDomanda(messaggio x, int primoNum, int secondoNum);

messaggio leggiComm(messaggio msg, char* input, int* contatti, int dim);


#endif	/* PARSER_H */

