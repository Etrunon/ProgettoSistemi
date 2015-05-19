#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/stat.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include "CONST.h"
#include "riparser.h"
#include "logica.h"
#include "gui.h"

void handler(int signo) {
    printf("Errore: %i\n", signo);
}

int main(int argc, char** argv) {
    maxWin = 20;
    maxClients = 10;

    initLogica();
    SetGUIMode(STANDARD_SERVER);
    updateScreen();

    giocatore *g = giocatoreConstructor();
    g->punteggio = 5;
    strcpy(g->name, "ciao");
    g->handlerFIFO = 7;
    g->IDGiocatore = 25;

    testStampaGiocatore(g);

    serverAggiungiGiocatore("Stefano", g->handlerFIFO);
    serverAggiungiGiocatore("Davide", g->handlerFIFO);
    serverAggiungiGiocatore("Samuel", g->handlerFIFO);
    serverAggiungiGiocatore("Matteo", g->handlerFIFO);
    serverAggiungiGiocatore("Alessandro", g->handlerFIFO);
    serverAggiungiGiocatore("Carlo", g->handlerFIFO);

    updateScreen();
    sleep(WAIT_TIME);
    testStampaClassifica("Con due player");


    serverAggiornaPunti(3, 1);
    serverAggiornaPunti(3, 1);
    serverAggiornaPunti(3, 1);

    updateScreen();
    sleep(WAIT_TIME);

    testStampaClassifica("Dopo aumento punti Samuel");

    serverAggiornaPunti(2, -1);
    serverAggiornaPunti(2, -1);
    serverAggiornaPunti(2, -1);

    updateScreen();
    sleep(WAIT_TIME);

    testStampaClassifica("Dopo toglimento punti ");

    togliGiocatore(6);

    testStampaClassifica("Tolto sam");

    serverAggiungiGiocatore("Luca", g->handlerFIFO);

    testStampaClassifica("Dopo tolto riaggiungo");


    //updateScreen();
    //sleep(WAIT_TIME);

    return (0);
}