#include "vineyard.c"
#include "util.c"
#include <stdio.h>
#include <time.h>
#include <locale.h>
#include <errno.h> // perror, errno
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        perror("Sorry ): ... Invalid number of arguments. 2 arguments were expected. Please, try again later. Thank you :)!\n");
        exit(1); // Ends the program.
    }

    strcpy(FILE_NAME, argv[1]);
    strcpy(PROGRAM_NAME, argv[0]);
    
    run();

    return 0;
}