#include <time.h>
#include <stdlib.h>

#include "logica.h"

void initLogica() {

}

void serverCambiaDomanda();

void serverGeneraClassifica(int* IDclients, int* punteggi);

int serverAggiungiGiocatore(char* nome, char* FIFO);

bool serverAggiornaPunti(int ID, int punti);

char* serverFIFOGiocatore(int ID);

void togliGiocatore(int ID);

void clientAggiornaPunti(int ID, int punti);

void clientNuovoGiocatore(char* nome, int ID, int punti);
