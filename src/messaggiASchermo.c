#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "messaggiASchermo.h"
#include "logica.h"

char messaggioASchermo [BUFFMESSAGGIO] = {};

void StampaNuovoGiocatore(char* name) {
    sprintf(messaggioASchermo, "%s%s\n", name, " si è aggiunto al gioco");
    aggiungiMessaggio(messaggioASchermo, false, ANSI_COLOR_BOLD_GREEN);
}

void StampaGiocatoreUscito(char* name) {
    sprintf(messaggioASchermo, "%s%s\n", name, " è uscito dal gioco");
    aggiungiMessaggio(messaggioASchermo, false, ANSI_COLOR_BOLD_GREEN);
}

void StampaServerPieno() {
    sprintf(messaggioASchermo, "%s\n", "Il server non ha posti disponibili!");
    aggiungiMessaggio(messaggioASchermo, true, ANSI_COLOR_RED);
}

void StampaBenvenutoClient(char* name) {
    sprintf(messaggioASchermo, "%s%s!\n", "Benvenuto in gioco, ", name);
    aggiungiMessaggio(messaggioASchermo, true, ANSI_COLOR_CYAN);
}

void StampaVittoria(char* name) {
    if (name == NULL)
        sprintf(messaggioASchermo, "%s\n", "Hai vinto!");
    else
        sprintf(messaggioASchermo, "%s%s\n", name, " ha vinto!");
    aggiungiMessaggio(messaggioASchermo, true, ANSI_COLOR_YELLOW);
}

void StampaRispostaInviata(int risposta) {
    sprintf(messaggioASchermo, "%s%i\n", "Hai risposto: ", risposta);
    aggiungiMessaggio(messaggioASchermo, false, NULL);
}

void StampaPartitaTerminata() {
    sprintf(messaggioASchermo, "%s\n", "Partita terminata!");
    aggiungiMessaggio(messaggioASchermo, true, ANSI_COLOR_YELLOW);
}

void StampaEsitoRisposta(char* nome, bool corretto) {
    if (corretto) {
        sprintf(messaggioASchermo, "%-7s%s\n", nome, " ha risposto correttamente");
    } else {
        sprintf(messaggioASchermo, "%-7s%s\n", nome, " ha sbagliato");
    }

    aggiungiMessaggio(messaggioASchermo, false, NULL);
}

void StampaDomandaModificata() {
    sprintf(messaggioASchermo, "%s\n", "Domanda modificata!");
    aggiungiMessaggio(messaggioASchermo, false, ANSI_COLOR_GREEN);
}

void StampaServerDisconnesso() {
    sprintf(messaggioASchermo, "%s\n", "Il server si è disconnesso!");
    aggiungiMessaggio(messaggioASchermo, true, ANSI_COLOR_RED);
}

void StampaTesting(char* messaggio) {
    sprintf(messaggioASchermo, "%s", "[");

    strcat(messaggioASchermo, messaggio);

    strcat(messaggioASchermo, "]");

    aggiungiMessaggio(messaggioASchermo, false, ANSI_COLOR_GREEN);
}

void StampaTestingGiocatore(int ID) {
    int index = cercaGiocatore(ID);

    StampaGiocatore(giocatoriCorrenti[index], messaggioASchermo);
    aggiungiMessaggio(messaggioASchermo, false, NULL);
}