/*
 * File:   main.c
 * Author: etrunon
 *
 * Created on April 30, 2015, 11:35 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

int main(int argc, char** argv) {
    int argument_value;

    int option_index = 0;

    static struct option long_options[] = {
        {"max", no_argument, 0, 'm'},
        {"win", required_argument, 0, 'w'},
        {"server", no_argument, 0, 's'},
        {"client", no_argument, 0, 'c'},
        {0, 0, 0, 0}
    };

    while (argument_value != -1) {
        argument_value = getopt_long(argc, argv, NULL, long_options, &option_index);
        switch (argument_value) {
            case 's':
            {
                printf("serverSide!\n");
                //server_side(argc, argv);
                break;
            }
            case 'm':
            {
                printf("Numero massino giocatori:\n",);
                //server_side(argc, argv);
                break;
            }

            case 'c':
            {
                printf("clientSide!\n");
                //client_side();
                break;
            }

            case -1: default:
            {
                printf("Errore nei parametri!\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    //aggiungo cose fichissime e belle
    return (EXIT_SUCCESS);
}

