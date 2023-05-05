#include "vineyard.c"
#include "util.c"
#include <stdio.h>
#include <time.h>
#include <locale.h>
#include <errno.h> // perror, errno

int main(int argc, char* argv[]){
    setlocale(LC_ALL, "pt_BR.UTF-8"); // Used to recognize special characters.

    if (argc != 2) {
        perror("Sorry ): ... Invalid number of arguments. 2 arguments were expected. Please, try again later. Thank you :)!\n");
        exit(1); // Ends the program.
    }

    FILE_NAME = (char*) malloc(sizeof(char)*(strlen(argv[1])));
    strcpy(FILE_NAME, argv[2]);

    run();

    free(FILE_NAME);

    return 0;
}