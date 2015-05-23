/*
 * File:   assetsGenerator.c
 * Author: mion00
 *
 * Created on May 17, 2015, 11:22 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define SERVERFILE "assets/ServerFile.txt"
#define CLIENTFILE "assets/ClientFile"

FILE* server;
FILE* client;
int numClients = 20;
int seed = 0;

/*
 *
 */

void assetServer() {

};

void rand_str(char *dest, size_t length) {
    char charset[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

void printName() {
    char name [50];
    int length = rand() % 6;
    rand_str(name, length);
    fprintf(client, "%s\n", name);
}

void printRisposte() {
    int numeroRisposte = 100 + (rand() % 100);
    srand(seed);
    int i = 0;
    for (i; i < numeroRisposte; i++) {
        fprintf(client, "%i\n", (rand() % 99) + (rand() % 99));
        /*
                if (i > 150) {
                    int uscita = rand() % RAND_MAX;
                    if (uscita >= RAND_MAX / 100 * 98) {
                        fprintf(client, "%s\n", "exit");
                        i = numeroRisposte;
                    }
                }
         */
    }
    fprintf(client, "%s\n", "exit");
}

void assetClient() {
    int i = 0;
    for (i = 0; i < numClients; i++) {
        char nomeFile [60] = {};
        sprintf(nomeFile, "%s%i%s", CLIENTFILE, i, ".txt");

        client = fopen(nomeFile, "w");
        if (client == NULL) {
            exit(EXIT_FAILURE);
        }

        printName();
        printRisposte();

        fclose(client);
    }
};

int main(int argc, char** argv) {
    printf("Creazione degli assets in corso.. \n");

    if (argc > 1) {
        seed = strtol(argv[1], NULL, 0);
    } else {
        seed = time(NULL);
    }

    srand(seed);

    /*
     * server = fopen(SERVERFILE, "w");
        if (server == NULL) {
            perror("");
            exit(EXIT_FAILURE);
        }
        assetServer();
     */

    assetClient();

    printf("Assets creati!\n");
    return (EXIT_SUCCESS);
}

