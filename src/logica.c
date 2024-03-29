/*
 * Progetto: Multiplayer Game
 * A.A 2014/2015
 * Carlo Mion   165878
 * Luca Bosotti 164403
 */

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "logica.h"

//Limite massimo di giocatori
int maxClients;
//Limite massimo di punti alla vittoria
int maxWin;
//Variabile con il numero di client presenti al momento nel gioco
int currentClients = 0;
//Inizializzazione della domanda in corso
domanda domandaCorrente = {};
//Inizializzazione a null dell'array dei giocatori correnti
giocatore* giocatoriCorrenti[11] = {};
//Variabile contenente il prossimo ID da assegnare a un client
int prossimoID = 1;

saveStat* storico[200] = {};
int indiceStorico = 0;

/**
 * Funzione costruttore che restituisce un puntatore a struct allocato dinamicamente e con tutti i
 * campi allocati vuoti e pronti da usare.
 * Ricordarsi di distruggere il giocatore finita la sua responsabilità
 * @return
 */
giocatore* giocatoreConstructor() {

    giocatore *g = (giocatore*) malloc(1 * (sizeof (giocatore)));
    g->name = (char*) malloc(MAXNAME * (sizeof (char)));
    return g;
}

/**
 * Funzione che distrugge un giocatore, passato per puntatore parametro, deallocando tutti i campi
 * dinamici.
 * @param g
 */
void giocatoreDestructor(giocatore *g) {
    free(g->name);
    free(g);
}

/**
 * Funzione di DEBUG che permette di stampare un giocatore con una formattazione leggibile e
 * mettendo in risalto i campi interni.
 * @param g puntatore al giocatore da stampare
 */
void* StampaGiocatore(giocatore *g, char* riempi) {
    sprintf(riempi, "%-6s%-10s %-5s%-5i %-10s%02i\n", "Nome", g->name, "ID", g->IDGiocatore, "Punti", g->punteggio);
}

/**
 * Funzione di DEBUG che stampa l'intera classifica dei giocatori presenti, stampando bene ogni
 * giocatore con la funzione testStampaGiocatore()
 * @param str
 */
void testStampaClassifica(char* str) {

    printf("%s\n", str);
    printf("Ecco la classifica\n");
    printf("Sono presenti %i giocatori su un massimo di %i\n\n", currentClients, maxClients);

    int i = 0;
    for (i; i < currentClients; i++) {
        StampaGiocatore(giocatoriCorrenti[i], NULL);
    }
    printf("\tFine classifica\n\n");
}

/**
 * Funzione che cambia la domanda corrente con una nuova
 */
void serverCambiaDomanda() {
    domandaCorrente.numero1 = rand() % 99;
    domandaCorrente.numero2 = rand() % 99;
}

/**
 * Funzione che inizializza la parte logica del SERVER
 */
void initLogica() {
    if (seed != -1)
        srand(seed);
    else
        srand(time(NULL));

    serverCambiaDomanda();
}

/**
 * Funzione che si assicura che la classifica sia ordinata per punteggio. Controlla se i punteggi
 * sono disordinati sia a destra che a sinistra (non va in loop per assunzioni sulla composizione
 * della classifica e sulla sua gestione). Dato un index di partenza (possibile punto di sbilanciamento)
 * vengono controllati i suoi vicini fino a quando
 *  -Non viene fatta nessuna modifica
 *  -Si arriva a uno dei due limite degli array
 * @param indicePartenza indice del punto di sbilanciamento
 * @return
 */
void swap(int index) {

    //puntatore di servizio
    giocatore *tmp;
    //variabile di uscita dal ciclo
    bool mosso;

    //Se c'è un client solo sarà sempre ordinato
    if (currentClients == 1)
        return;

    // In caso di più client comincia a controllare
    do {

        //inizializzata a falso per uscire subito nel caso di situazione bilanciata
        mosso = false;

        //Se il giocatore index e il suo vicino di destra esiste
        if (giocatoriCorrenti[index] != NULL && giocatoriCorrenti[index + 1] != NULL) {

            //E se i punti di index sono minori dei punti del vicino di destra e NON sono sul limite destro dell'array
            if (giocatoriCorrenti[index]->punteggio < giocatoriCorrenti[index + 1]->punteggio && index < currentClients) {

                //Allora swappo i due giocatori usando anche tmp
                tmp = giocatoriCorrenti[index];
                giocatoriCorrenti[index] = giocatoriCorrenti[index + 1];
                giocatoriCorrenti[index + 1] = tmp;
                //Ho applicato una modifica quindi devo ricontrollare
                mosso = true;
                //Aggiorno l'index per tenere controllato il punto di sbilanciamento
                index++;
            }
        }

        //Se il giocatore index e il suo vicino di sinistra esiste
        if (giocatoriCorrenti[index] != NULL && giocatoriCorrenti[index - 1] != NULL) {
            //E se i punti di index sono maggiori dei punti del vicino di sinistra e NON sono sul limite sinistro dell'array
            if (index != 0 && giocatoriCorrenti[index - 1]->punteggio < giocatoriCorrenti[index]->punteggio) {

                //Allora swappo i due giocatori usando anche tmp
                tmp = giocatoriCorrenti[index];
                giocatoriCorrenti[index] = giocatoriCorrenti[index - 1];
                giocatoriCorrenti[index - 1] = tmp;
                //Ho applicato una modifica quindi devo ricontrollare
                mosso = true;
                //Aggiorno l'index per tenere controllato il punto di sbilanciamento
                index--;
            }
        }
        //Controllo se ho applicato modifiche, potrei ancora essere sbilanciato
    } while (mosso == true);
}

/**
 * Funzione che riempie gli array passati per parametro con i punteggi dei giocatori presenti.
 * Assume che la classifica sia già correttamente ordinata.
 * @param IDclients array con gli id
 * @param punteggi  array con i punti
 */
void serverGeneraClassifica(int* IDclients, int* punteggi) {

    int i = 0;
    for (i; i < currentClients; i++) {
        if (giocatoriCorrenti[i] != NULL) {
            IDclients[i] = giocatoriCorrenti[i]->IDGiocatore;
            punteggi[i] = giocatoriCorrenti[i]->punteggio;
        }
    }
}

/**
 * Aggiunge un giocatore, ritorna il suo ID
 * Se non c'è spazio, ritorna -1
 * @param nome stringa col nome del client da aggiungere
 * @param handlerFIFO handler del nuovo giocatore
 * @return ID del client aggiunto
 */
int serverAggiungiGiocatore(char* nome, int handlerFIFO) {

    if (currentClients >= maxClients)
        return -1;

    //Creo un giocatore e lo riempio coi dati in input
    giocatore* g = giocatoreConstructor();
    g->handlerFIFO = handlerFIFO;
    strcpy(g->name, nome);
    g->IDGiocatore = prossimoID;
    prossimoID++;
    g->punteggio = maxClients - currentClients - 1;

    //Assegno il giocatore alla struttura dati
    giocatoriCorrenti[currentClients] = g;

    currentClients++;
    swap(currentClients - 1);

    //Ritorno l'id assegnato al client
    return prossimoID - 1;
}

/**
 * Funzione che dato un ID di un giocatore lo cerca dentro la struttura dati.
 * Ritorna -1 se non lo trova, altrimenti il suo indice all'interno dell'array
 * @param ID
 * @return  indice nell'array
 */
int cercaGiocatore(int ID) {

    int i = 0;
    for (i; i < currentClients; i++) {
        if (giocatoriCorrenti[i]->IDGiocatore == ID)
            return i;
    }

    return -1;
}

void swapStat(int index) {

    //puntatore di servizio
    saveStat *tmp;
    //variabile di uscita dal ciclo
    bool mosso;

    //Se c'è un client solo sarà sempre ordinato
    if (indiceStorico == 1)
        return;

    // In caso di più client comincia a controllare
    do {

        //inizializzata a falso per uscire subito nel caso di situazione bilanciata
        mosso = false;

        //Se il giocatore index e il suo vicino di destra esiste
        if (storico[index] != NULL && storico[index + 1] != NULL) {

            //E se i punti di index sono minori dei punti del vicino di destra e NON sono sul limite destro dell'array
            if (storico[index]->g->punteggio < storico[index + 1]->g->punteggio && index < indiceStorico) {

                //Allora swappo i due giocatori usando anche tmp
                tmp = storico[index];
                storico[index] = storico[index + 1];
                storico[index + 1] = tmp;
                //Ho applicato una modifica quindi devo ricontrollare
                mosso = true;
                //Aggiorno l'index per tenere controllato il punto di sbilanciamento
                index++;
            }
        }

        //Se il giocatore index e il suo vicino di sinistra esiste
        if (storico[index] != NULL && storico[index - 1] != NULL) {
            //E se i punti di index sono maggiori dei punti del vicino di sinistra e NON sono sul limite sinistro dell'array
            if (index != 0 && storico[index - 1]->g->punteggio < storico[index]->g->punteggio) {

                //Allora swappo i due giocatori usando anche tmp
                tmp = storico[index];
                storico[index] = storico[index - 1];
                storico[index - 1] = tmp;
                //Ho applicato una modifica quindi devo ricontrollare
                mosso = true;
                //Aggiorno l'index per tenere controllato il punto di sbilanciamento
                index--;
            }
        }
        //Controllo se ho applicato modifiche, potrei ancora essere sbilanciato
    } while (mosso == true);
}

void salvaStat(giocatore* g, char *time) {

    saveStat *sv = (saveStat*) malloc(1 * sizeof (saveStat));
    sv->g = g;
    sv->time = (char*) malloc((strlen(time) + 1) *(sizeof (char)));
    strcpy(sv->time, time);

    storico[indiceStorico] = sv;
    swapStat(indiceStorico);
    indiceStorico++;
}

/**
 * Funzione che aggiunge o toglie UN punto solo. Lancia automaticamente un ordinamento della classifica
 * Ritorna true se il giocatore in questione ha vinto
 * @param ID
 * @param punti
 * @return
 */
bool serverAggiornaPunti(int ID, int punti) {

    int index = cercaGiocatore(ID);

    //Se il giocatore va sotto zero, lo riporto a zero
    if (giocatoriCorrenti[index]->punteggio + punti <= -1) {
        giocatoriCorrenti[index]->punteggio = 0;
    } else
        //Il giocatore non è andato sotto quindi...
        giocatoriCorrenti[index]->punteggio += punti;

    //Controllo se devo riordinare la classifica


    //Se il giocatore ha vinto allora ritorno true, altrimenti false
    if (giocatoriCorrenti[index]->punteggio >= maxWin) {

        swap(index);
        return true;
    } else {
        swap(index);
        return false;

    }
}

/**
 * Funzione che dato un ID ritorna l'handler della sua FIFO
 * @param ID bersaglio
 * @return handler della FIFO
 */
int serverFIFOGiocatore(int ID) {

    int i = cercaGiocatore(ID);
    return giocatoriCorrenti[i]->handlerFIFO;
}

/*FUNZIONI DA USARE LATO CLIENT*/

/**
 * Funzione che TODO
 * @param ID del giocatore bersaglio
 * @param punti da modificare
 */
void clientAggiornaPunti(int ID, int punti) {

    int i = cercaGiocatore(ID);
    giocatoriCorrenti[i]->punteggio = punti;
    swap(i);
}

/**
 * Funzione che aggiunge il giocatore specificato alla lista di quelli presenti nella partita insieme al client
 * Vengono settati il nome, l'ID e i punti (non come delta ma come set bruto)
 * @param nome
 * @param ID
 * @param punteggio
 */
void clientAggiungiGiocatore(char* nome, int ID, int punteggio) {

    //Creo un giocatore e lo riempio coi dati in input
    giocatore* g = giocatoreConstructor();
    strcpy(g->name, nome);
    g->IDGiocatore = ID;
    g->punteggio = punteggio;

    //Assegno il giocatore alla struttura dati
    giocatoriCorrenti[currentClients] = g;
    currentClients++;
    swap(currentClients - 1);
}

/*METODI COMUNI AL SERVER E AL CLIENT*/

/**
 * Metodo che toglie il giocatore specificato dalla lista di quelli presenti a seguito per esempio di un
 * logout. Riordina la lista di giocatori presenti.
 * @param ID
 */
void togliGiocatore(int ID, char* timestring) {

    //Prendo l'indice nell'array della clasifica del giocatore
    int i = cercaGiocatore(ID);
    if (i < 0)
        return;
    //Metto in una var temporanea i suoi punti per poterli ripristinare nello storico
    int puntiTmp = giocatoriCorrenti[i]->punteggio;

    giocatore *gtmp = giocatoriCorrenti[i];
    salvaStat(gtmp, timestring);

    //Gli setto il punteggio a -1 per farlo scalare nell'ultima posizione
    giocatoriCorrenti[i]->punteggio = -1;
    //Applico l'ordinamento
    swap(i);

    //Una volta scalato gli devo ripristinare i punti
    gtmp->punteggio = puntiTmp;

    //Decremento i client collegati
    currentClients--;

    //Tolgo il giocatore dalla classifica
    giocatoriCorrenti[currentClients] = NULL;
}

/**
 * Getter del nome del giocatore. Riempie una stringa sufficientemente lunga passata per puntatore.
 * @param ID
 * @param riempi
 */
void getNomeGiocatore(int ID, char* riempi) {

    int i = cercaGiocatore(ID);
    if (i >= 0)
        strcpy(riempi, giocatoriCorrenti[i]->name);
}

/**
 * Getter dei punti del giocatore con ID.
 * @param ID
 * @return punti
 */
int getPuntiGiocatore(int ID) {

    int i = cercaGiocatore(ID);
    return giocatoriCorrenti[i]->punteggio;
}