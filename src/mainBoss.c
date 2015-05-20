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

void handler(int signo) {
    printf("Errore: %i\n", signo);
}

int main(int argc, char** argv) {

    return (0);
}