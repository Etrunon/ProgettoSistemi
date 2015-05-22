/*
 * File:   messaggiASchermo.h
 * Author: mion00
 *
 * Created on May 22, 2015, 11:46 AM
 */

#ifndef MESSAGGIASCHERMO_H
#define	MESSAGGIASCHERMO_H

#include "guiMessages.h"
#include "CONST.h"

typedef enum language {
    ITA
} language;

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

void StampaTesting(char* messaggio);

void StampaTestingGiocatore(int ID);

#endif	/* MESSAGGIASCHERMO_H */

