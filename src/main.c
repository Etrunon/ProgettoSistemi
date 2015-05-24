/*
 * Progetto: Multiplayer Game
 * A.A 2014/2015
 * Carlo Mion   165878
 * Luca Bosotti 164403
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "server.h"
#include "client.h"
#include "gui.h"
#include "CONST.h"

/*Variabile globale per sapere se siamo in modalità testing
 *Impostata dal main e poi usata in sola lettura nel resto del programma*/
int testing = 0;
int seed = -1;

/*Stampa la spiegazione dei parametri*/
void explainParameters() {
    printf("Per avviare il server, usare l'opzione --server, con argomenti --max <numero massimo giocatori> e --win <punteggio per vittoria>\n");
    printf("Per avviare il client, usare l'opzione --client\n");
}

/* Controlla che i parametri siano validi, abortendo il programma in caso contrario
 * In base ai parametri invocati, avvia un server od un client
 */
int main(int argc, char** argv) {

    /*Indica se sono il server od il client*/
    int server = -1;

    /*Valori di default nel caso in cui non siamo specificati come argomenti*/
    int maxClients = 10;
    int winPoints = 10;

    if (argc == 1) {
        explainParameters();
        return (EXIT_FAILURE);
    }

    int option_index = 0;

    /*Lista dei parametri accettati*/
    struct option long_options[] = {
        {"max", required_argument, NULL, 'm'},
        {"win", required_argument, NULL, 'w'},
        {"server", no_argument, &server, 1},
        {"client", no_argument, &server, 0},
        {"testing", no_argument, &testing, 1},
        {"seed", required_argument, NULL, 's'},
        {NULL, 0, NULL, 0}
    };

    int argument_value;
    while ((argument_value = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
        switch (argument_value) {
            case 0: case 1:
                break;
            case 's':
            {
                seed = strtol(optarg, NULL, 10);
            }
                break;
            case 'm':
            {
                int value = strtol(optarg, NULL, 10);
                if (value < 1 || value > 10) {
                    printf("Il numero di giocatori deve essere compreso tra 1 e 10!\n");
                    exit(EXIT_FAILURE);
                }
                maxClients = value;
            }
                break;
            case 'w':
            {
                int value = strtol(optarg, NULL, 10);
                if (value < 10 || value > 100) {
                    printf("Il punteggio deve essere compreso tra 10 e 100!\n");
                    exit(EXIT_FAILURE);
                }
                winPoints = value;
            }
                break;
            case '?':
                break;
            default:
            {
                explainParameters();
                return (EXIT_FAILURE);
            }
                break;
        }
    }
    if (server == -1) {
        explainParameters();
        exit(EXIT_FAILURE);
    }
    if (server) {
        /*Avvio il server*/
        initServer(maxClients, winPoints);
    } else {
        /*Avvio il client*/
        initClient();
    }
    return (EXIT_SUCCESS);
}

