#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "server.h"
#include "client.h"
#include "gui.h"

void explainParameters() {
    printf("Per avviare il server, usare l'opzione --server, con argomenti --max <numero massimo giocatori> e --win <punteggio per vittoria>\n");
    printf("Per avviare il client, usare l'opzione --client\n");
}

/* Controlla che i parametri siano validi, abortendo il programma in caso contrario
 * In base ai parametri invocati, avvia un server od un client
 */
int main(int argc, char** argv) {

    int server = 0;
    int testing = 0;
    int maxClients = 10;
    int winPoints = 10;

    if (argc == 1) {
        explainParameters();
        return (EXIT_FAILURE);
    }

    int option_index = 0;

    struct option long_options[] = {
        {"max", required_argument, NULL, 'm'},
        {"win", required_argument, NULL, 'w'},
        {"server", no_argument, &server, 1},
        {"client", no_argument, &server, 0},
        {"testing", no_argument, &testing, 1},
        {NULL, 0, NULL, 0}
    };

    int argument_value;
    while ((argument_value = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
        switch (argument_value) {
            case 0: case 1:
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

    if (server) {
        initServer(maxClients, winPoints, testing);
    } else {
        initClient(testing);
    }
    return (EXIT_SUCCESS);
}

