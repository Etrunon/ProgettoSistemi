/* 
 * File:   parser.h
 * Author: etrunon
 *
 * Created on May 8, 2015, 12:06 PM
 */

#ifndef PARSER_H
#define	PARSER_H

struct messaggio;

/**
 * Ciao
 * @return 
 */
char* crInvDatiRisp(int pid);

/**
 * 
 * @return 
 */
char* crRichPartec(int pid);

/**
 * 
 * @return 
 */
char* crInvLogOut(int pid);

/**
 * 
 * @return 
 */
char* crMesgCorrotto(int pid);

/**
 * 
 * @return 
 */
char* crAccettaClient(int pid);

/**
 * 
 * @return 
 */
char* crRifiutaClient(int pid);

/**
 * 
 * @return 
 */
char* crInvClassifica(int pid);

/**
 * 
 * @return 
 */
char* crBroadNuovoGiocatore(int pid);

/**
 * 
 * @return 
 */
char* crBroadAggPunti(int pid);

/**
 * 
 * @return 
 */
char* crInvDomanda(int pid);

/**
 * Funzione che dato l'array dei processi conosciuti con cui si è in contatto controlla se il mittente è nella lista.
 * @param contatti
 * @return 
 */
bool checkPid(int pid, int* contatti);


messaggio leggiMessaggio(char* input);


#endif	/* PARSER_H */

