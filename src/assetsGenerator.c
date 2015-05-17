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
int numClients;

/*
 *
 */

void assetServer() {

};

void rand_str(char *dest, size_t length) {
    char charset[] = "0123456789"
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

void printName() {
    char name [30];
    int length = rand() % 30;
    rand_str(name, length);
    fprintf(client, "%s\n", name);
}

void printRisposte() {
    int numeroRisposte = rand() % 200;
    int i = 0;
    for (i; i < numeroRisposte; i++) {
        fprintf(client, "%i\n", rand() % 200);
        int uscita = rand() % RAND_MAX;
        if (uscita >= RAND_MAX / 100 * 97) {
            fprintf(client, "%s\n", "exit");
        }
    }
}

void assetClient() {
    int i = 0;
    for (i; i < numClients; i++) {
        char nomeFile [20];
        sprintf(nomeFile, "%s%i%s", CLIENTFILE, i, ".txt");

        client = fopen(nomeFile, "w");
        if (client == NULL) {
            perror("");
            exit(EXIT_FAILURE);
        }
        printName();
        printRisposte();
    }
};

int main(int argc, char** argv) {
    printf("Creazione degli assets in corso.. \n");
    numClients = 1;
    srand(time(NULL));

    if (argc > 1) {
        numClients = strtol(argv[1], NULL, 0);
    }
    server = fopen(SERVERFILE, "w");
    if (server == NULL) {
        perror("");
        exit(EXIT_FAILURE);
    }

    assetServer();
    assetClient();

    printf("Assets creati!\n");
    return (EXIT_SUCCESS);
}

