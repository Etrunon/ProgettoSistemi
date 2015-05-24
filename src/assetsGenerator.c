/*
 * Progetto: Multiplayer Game
 * A.A 2014/2015
 * Carlo Mion   165878
 * Luca Bosotti 164403
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define SERVERFILE "assets/ServerFile.txt"
#define CLIENTFILE "assets/ClientFile"

/*Variabili usate per scrivere i file da usare come input per i client in testing*/
FILE* client;
int numClients = 20;
int seed = 0;

/*Stampa una stringa casuale, scegliendo tra i caratteri dell'array charset[]*/
void rand_str(char *dest, size_t length) {
    char charset[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

/*Stampa nella prima riga del file un nome casuale, usato per identificare il client*/
void printName() {
    char name [50];
    int length = rand() % 6;
    rand_str(name, length);
    fprintf(client, "%s\n", name);
}

/*stampa un numero variabile di risposte, una per riga, usando un seed prestabilito*/
void printRisposte() {
    int numeroRisposte = 100 + (rand() % 100);
    /*Inizializza il seed perché corrisponda con quello usato dal server per generare le domande*/
    srand(seed);
    int i = 0;
    for (i; i < numeroRisposte; i++) {
        fprintf(client, "%i\n", (rand() % 99) + (rand() % 99));
    }
    /*Dopo aver stampato la lista di risposte, stampa un comando di uscita, che fa terminare il client*/
    fprintf(client, "%s\n", "exit");
}

/*Genera negli asssets un file per ogni client*/
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

/*Genera i file di assets usati dai client come input durante il testing*/
int main(int argc, char** argv) {
    printf("Creazione degli assets in corso.. \n");

    if (argc > 1) {
        /*Ho ricevuto il seed come parametro, e lo uso per generare le risposte*/
        seed = strtol(argv[1], NULL, 0);
    } else {
        /*Non ho seed come parametro, e uso il tempo attuale come seed*/
        seed = time(NULL);
    }

    srand(seed);

    assetClient();

    printf("Assets creati!\n");
    return (EXIT_SUCCESS);
}

