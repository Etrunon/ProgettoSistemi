/*
 * Progetto: Multiplayer Game
 * A.A 2014/2015
 * Carlo Mion   165878
 * Luca Bosotti 164403
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "client.h"
#include "CONST.h"
#include "allFifo.h"
#include "parser.h"
#include "commands.h"
#include "gui.h"
#include "logica.h"
#include "guiMessages.h"
#include "messaggiASchermo.h"

/*Interi che contengono gli handler per le FIFO di ascolto del client e di scrittura verso il server*/
int ascoltoDalServer;
int scriviAlServer;
/*Path alla FIFO del client*/
char clientFifo [MAX_FIFONAME];

/*ID e nome del client in gioco*/
int clientID;
char name[MAXNAME];

/*Variabile usata per indicare se il giocatore è stato accettato dal server nel gioco
 Usata dalla GUI per cambiare la modalità di display e dal thread di input utente per mostrare il prompt appropriato*/
bool connesso = false;

/*Stringa usata per scrivere messaggi a schermo, passata di volta in volta al modulo GuiMessages*/
char msgTmp [BUFFMESSAGGIO];

/**
 * Funzione che avverte il server del logout del client
 */
void avvisaServer() {
    messaggio* logout = messaggioConstructor(clientID, LOGOUT_AL_SERVER);
    inviaMessaggio(scriviAlServer, logout);
    messaggioDestructor(logout);
}

/*
 * Chiude la FIFO ed eventuali altre risorse rimaste aperte
 * Chiamata nel momento in cui il client sta termimando la sua esecuzione come signal handler
 */
void cleanupClient(int sig) {
    sprintf(msgTmp, "%s\n", "Client disattivato");
    SetGUIMode(EXIT_CLIENT);
    aggiungiMessaggio(msgTmp, true, ANSI_COLOR_CYAN);

    if (connesso) {
        /*Se sono connesso, avviso il server che me ne sto andando*/
        avvisaServer();
    }

    /*Chiudo la FIFO di ascolto dal server e la rimuovo dal file system*/
    close(ascoltoDalServer);
    unlink(clientFifo);
    exit(EXIT_SUCCESS);
}

/*Signal Handler che gestisce la disconnesione del server, se dovesse avvenire in maniera inaspettata, sfruttando il segnale SIGPIPE*/
void serverDisconnesso(int sig) {
    //StampaServerDisconnesso();
    cleanupClient(0);
}

/**
 * Funzione avviata come thread per ascoltare l'input utente da terminale ed eseguire i comandi impartiti
 * @param arg
 * @return
 */
void * inputUtenteClient(void* arg) {

    /*Come prima cosa, stampa il messaggio di help, che riporta i comandi disponibili*/
    printHelp(false);

    /*Struct che permette di comprendere il tipo di comando impartito*/
    comando c;
    /*Struct che contiene i dati inseriti in input dall'utente, come la risposta o il nome*/
    data d;

    do {
        /*Chiamata bloccante in attesa di input utente, ritorna il comando impartito e inserisce eventuali dati nella struct d*/
        c = leggiInput(&d);

        switch (c) {
                /*In base al comando impartito, agisco di conseguenza*/
            case RISPOSTA:
            {
                //Se l'input è una risposta E sono connesso a una partita, allora cerco di inviarla
                if (connesso) {
                    /*Stampo a schemo la risposta inserita dall'utente, per avere un feedback visuale*/
                    StampaRispostaInviata(d.risposta);
                    messaggio* msg = messaggioConstructor(clientID, INVIA_RISPOSTA);

                    if (testing) {
                        /*Introduciamo un po' di latenza variabile sulle risposte dei client al server in modalità testing
                        /*Per non sovvraccaricare la FIFO e poter controllare l'andamento del testing, senza essere sommersi da troppe print di informazione*/
                        struct timespec intervallo, intervallo2;
                        intervallo.tv_sec = 0;
                        intervallo.tv_nsec = 200000000 + 100000000 * (rand() % 30);
                        nanosleep(&intervallo, &intervallo2);

                        /*I client hanno una percentuale casuale di indovinare la risposta*/
                        int giusto = rand() % 100;
                        if (giusto >= 10)
                            msg->risposta = d.risposta;
                        else
                            msg->risposta = d.risposta + giusto;
                    } else {
                        msg->risposta = d.risposta;
                    }

                    inviaMessaggio(scriviAlServer, msg);
                    messaggioDestructor(msg);
                }
            }
                break;
            case LOG_EXIT:
                /*Il client vuole uscire dalla modalità di log*/
            {
                if (connesso)
                    /*Se sono connesso, mostra la schermata di default con il grafico dei punteggi*/
                    SetGUIMode(STANDARD_CLIENT);
                else
                    /*Se non sono connesso, mostra la schermata di login, chiedente il nome giocatore*/
                    SetGUIMode(LOGIN_CLIENT);
            }
                break;
            case HELP:
            {
                /*L'utente ha chiesto la lista di comandi disponibili*/
                printHelp(false);
            }
                break;
            case NOME:
            {
                /*L'utente ha inserito una stringa, che devo usare come nome
                 A questo punto posso richiedere la partecipazione al server, se non sono già connesso*/
                if (!connesso) {
                    strcpy(name, d.nome);

                    /*Richiedo la partecipazione al server*/
                    messaggio* m = messaggioConstructor(clientID, RICHIESTA_PARTECIPAZIONE);
                    sprintf(m->pathFifo, "%s", clientFifo);
                    sprintf(m->nomeClient, "%s", d.nome);
                    inviaMessaggio(scriviAlServer, m);

                    messaggioDestructor(m);

                } else {
                    /*Sono connesso al server, ma il giocatore ha inserito una stringa che non posso valutare come nome,
                     essendo già connesso al server*/
                    sprintf(msgTmp, "%s\n", "Input non valido");
                    aggiungiMessaggio(msgTmp, true, ANSI_COLOR_RED);
                }
            }
                break;
            case ERRORE:
                /*Il giocatore ha inserito un comando che non riesco ad interpretare come valido*/
            {
                sprintf(msgTmp, "%s\n", "Input non valido");
                aggiungiMessaggio(msgTmp, true, ANSI_COLOR_RED);
            }
                break;
            default:
                break;
        }

        /*Aggiorna la schermata dopo aver processato l'input ricevuto*/
        updateScreen();
        /*Rimane in ascolto di input utente fino a quando non ricevo il comando di chiusura*/
    } while (c != CHIUSURA);

    /*Chiudo il client*/
    cleanupClient(0);

    return NULL;
}

/*Rimane in attesa di messaggi dal server, tramite la FIFO di ascolto*/
void ascoltaServer() {
    while (1) {

        //Creo il messaggio vuoto da riempire con il contenuto ricevuto dalla FIFO
        messaggio* msg = messaggioConstructor(0, 0);
        leggiMessaggio(ascoltoDalServer, msg);

        //A seconda del codice del messaggio eseguo l'azione correlata
        switch (msg->codiceMsg) {
            case RIFIUTA_CLIENT:
            {
                //Il server ha rifiutato il login
                StampaServerPieno();
                cleanupClient(0);
            }
                break;
            case ACCETTA_CLIENT:
            {
                //Il server ha accettato il login
                connesso = true;

                if (testing) {
                    /*Se sono in fase di testing, attendo che tutti gli altri client vengano accettati in gioco,
                     prima di cominciare ad inviare le rispsote*/
                    sleep(3);
                    /*Avvio il thread dedicato all'input utente, che in fase di testing è stato dirottato al file negli asssets*/
                    pthread_t threadID;
                    pthread_create(&threadID, NULL, &inputUtenteClient, NULL);
                }

                //Una volta connesso, si cambia l'interfaccia
                SetGUIMode(STANDARD_CLIENT);
                //Aggiungo l'ID assegnatomi dal server ed il punteggio di vittoria alle mie variabili globali
                clientID = msg->IDOggetto;
                maxWin = msg->maxWin;
                //Mi aggiungo ai giocatori presenti nella partita nella mia stuttura interna di gioco
                clientAggiungiGiocatore(name, clientID, msg->punti);
                //Stampo messaggio di benvenuto
                StampaBenvenutoClient(name);
            }
                break;
            case NUOVO_GIOCATORE_ENTRATO:
            {
                //E' arrivato un nuovo giocatore nella partita. Si aggiungono i suoi dati alla struttura di gioco interna
                strcpy(name, msg->nomeClient);
                clientAggiungiGiocatore(name, msg->IDOggetto, msg->punti);
                /*Stampo a video l'arrivo del nuovo giocatore*/
                StampaNuovoGiocatore(msg->nomeClient);

            }
                break;
            case GIOCATORE_USCITO:
            {
                //Un giocatore è uscito dalla partita e devo toglierlo dalla struttura di gioco interna

                getNomeGiocatore(msg->IDOggetto, name);
                togliGiocatore(msg->IDOggetto, msg->timestring);
                /*Stampo a video che è uscito un giocatore*/
                StampaGiocatoreUscito(name);
            }
                break;
            case ESITO_RISPOSTA:
            {
                //Dopo aver mandato una risposta, il server mi ha mandato l'esito
                clientAggiornaPunti(clientID, msg->punti);
                if (msg->corretta) {
                    /*Se la risposta è corretta*/
                    sprintf(msgTmp, "%s\n", "Risposta corretta!");
                } else {
                    /*Se la risposta è sbagliata*/
                    sprintf(msgTmp, "%s\n", "Risposta sbagliata!");
                }
                aggiungiMessaggio(msgTmp, false, NULL);
            }
                break;
            case MODIFICA_PUNTEGGIO_GIOCATORE:
            {
                //Il punteggio di qualcun altro è cambiato, devo aggiornarlo nella mia struttura di gioco interna
                clientAggiornaPunti(msg->IDOggetto, msg->punti);

                if (msg->corretta) {
                    /*Se qualcuno ha risposto correttamente, lo indico a schermo*/
                    char tmp [MAXNAME];
                    getNomeGiocatore(msg->IDOggetto, tmp);
                    StampaEsitoRisposta(tmp, true);
                }
            }
                break;
            case INVIA_DOMANDA:
            {
                //Il server mi ha inviato una nuova domanda
                domandaCorrente.numero1 = msg->domanda1;
                domandaCorrente.numero2 = msg->domanda2;
                /*Avviso a schermo che la domanda è stata modificata*/
                StampaDomandaModificata();
            }
                break;
            case VITTORIA:
            {
                //Qualcuno (anche io forse) ha vinto!
                //Controllo se ho vinto io
                if (msg->IDOggetto == clientID) {
                    /*Se io stesso ho vinto, lo stampo a schermo*/
                    StampaVittoria(NULL);
                } else {
                    //Non ho vinto io :(, stampo il nome di chi ha vinto
                    char tmp [MAXNAME];
                    getNomeGiocatore(msg->IDOggetto, tmp);
                    StampaVittoria(tmp);
                }

                StampaPartitaTerminata();
                //Termino l'esecuzione del client
                cleanupClient(0);
            }
                break;
            case SERVER_SPEGNIMENTO:
            {
                //Il server si è spento, avvisandomi. Fine dei giochi
                StampaServerDisconnesso();
                cleanupClient(0);
            }
                break;
            default: break;

        }
        /*Distruggo l'istanza di messaggio usata dopo aver intraprso l'azione correlata, per evitare memory leak*/
        messaggioDestructor(msg);
        updateScreen();
    }
}

/*Funzione chiamata dal main per avviare il processo come client*/
int initClient() {

    /*Segnali di chiusura*/
    signal(SIGSEGV, cleanupClient);
    signal(SIGABRT, cleanupClient);
    /*Chiusura terminale*/
    signal(SIGHUP, cleanupClient);
    signal(SIGTERM, cleanupClient);
    /*Ctrl-C*/
    signal(SIGINT, cleanupClient);
    /*Ctrl-Z*/
    signal(SIGTSTP, cleanupClient);
    signal(SIGSTOP, cleanupClient);
    /*Ctrl-\*/
    signal(SIGQUIT, cleanupClient);

    /*Segnali di chiusura FIFO, se perdo collegamento con il server brutalmente*/
    signal(SIGPIPE, serverDisconnesso);

    /*Inizializzo GUI*/
    if (testing) {
        SetGUIMode(TESTING_CLIENT);
    } else {
        calcolaLarghezzaSchermo(0);
        SetGUIMode(LOGIN_CLIENT);
    }
    updateScreen();

    /*Controllo se esiste un server*/;
    int exist = access(SERVERPATH, F_OK);

    //Se non esiste un server, lo notifico a schermo e chiudo
    if (exist == -1) {
        SetGUIMode(EXIT_CLIENT);
        sprintf(msgTmp, "%s\n", "Il server non è attivo!");
        aggiungiMessaggio(msgTmp, true, ANSI_COLOR_RED);
        return -1;
    }


    /*Apro la mia FIFO da cui ascolterò i messaggi dal server*/
    sprintf(clientFifo, "%s%i", CLIENTFIFO, getpid());
    ascoltoDalServer = creaFifoLettura(clientFifo);
    if (ascoltoDalServer == -1) {
        /*Se non riesco ad aprire la FIFO, abortisco*/
        sprintf(msgTmp, "%s\n", "Errore nell'apertura del client");
        aggiungiMessaggio(msgTmp, true, ANSI_COLOR_RED);
        cleanupClient(0);
        exit(EXIT_FAILURE);
    }

    /*Apro la FIFO per contattare il server*/
    scriviAlServer = creaFiFoScrittura(SERVERPATH);
    if (scriviAlServer == -1) {
        sprintf(msgTmp, "%s\n", "Errore nell'apertura del client");
        aggiungiMessaggio(msgTmp, true, ANSI_COLOR_RED);
        cleanupClient(0);
    }

    if (!testing) {
        /*Faccio partire il thread di ascolto dei messaggi da terminale, se non sono in modalità testing*/
        pthread_t threadID;
        pthread_create(&threadID, NULL, &inputUtenteClient, NULL);
    } else {
        /*Sono in modalità testing. Come prima cosa, rihiedo la partecipazione al server,
         * e solo quando avrò ricevuto il suo messaggio di accettazione avvierò il thread di lettura input utente*/
        messaggio* partecipazione = messaggioConstructor(0, RICHIESTA_PARTECIPAZIONE);
        sprintf(partecipazione->pathFifo, "%s", clientFifo);
        char nome [MAXNAME];
        /*Leggo il nome da input utente, che fa riferimento ad uno dei file di assets*/
        scanf("%s", nome);
        sprintf(partecipazione->nomeClient, "%s", nome);
        inviaMessaggio(scriviAlServer, partecipazione);
        messaggioDestructor(partecipazione);
    }

    /*Ascolto FIFO da cui riceverò i messaggi dal server. Il thread principale rimane all'interno di questo metodo*/
    ascoltaServer();


    cleanupClient(0);
    return 0;
}
