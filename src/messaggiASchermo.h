/*
 * Progetto: Multiplayer Game
 * A.A 2014/2015
 * Carlo Mion   165878
 * Luca Bosotti 164403
 */


#ifndef MESSAGGIASCHERMO_H
#define	MESSAGGIASCHERMO_H

#include "guiMessages.h"
#include "CONST.h"

/* Librearia che permette di centralizzare la gestione dei messaggi a schermo
 * I nomi delle funzioni sono esplicativi del messaggio che stampano.
 */
void StampaNuovoGiocatore(char* name);
void StampaGiocatoreUscito(char* name);
void StampaServerPieno();
void StampaBenvenutoClient(char* name);
void StampaVittoria(char* name);
void StampaPartitaTerminata();
void StampaEsitoRisposta(char* nome, bool corretto);

void StampaRispostaInviata(int risposta);
void StampaDomandaModificata();
void StampaServerDisconnesso();

/*Usate in fase di testing*/
void StampaTesting(char* messaggio);
void StampaTestingGiocatore(int ID);

#endif	/* MESSAGGIASCHERMO_H */

