#include "util.h"

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* === START UP OF THE APPLICATION === */
void run() {
    
    char option;

    do {
        header();

        printf("WELCOME TO VINEYARD WORKFORCE PROCESS MANAGER ::::::\n");
        printf("Options: \n");
        printf("\t:: [A] => Add\n");
        printf("\t:: [D] => Delete\n");
        printf("\t:: [M] => Modify\n");
        printf("\t:: [L] => List by days\n");
        printf("\t:: [S] => Start buses\n");
        printf("\t:: [E] => Exit");

        printf("\n\tEnter the operation: ");
        scanf("%c", &option);
        option = toupper(option);
        fgetc(stdin); /* Remove enter in the buffer. */

        switch (option) {
            case 'A': 
                add();
                break;
            case 'D':
                remove_applicant();
                break;
            case 'M':
                modify_applicant();
                break;
            case 'L':
                list_applicants_by_days();
                break;
            case 'S':
                start_buses();
                break;
            case 'E':
                dismiss();
                break;    
            default:
                printf("\nInvalid option!\n");
                break;
        }
    } while (option != 'E');
}

/* === PRINT NICE HEADER :) === */
void
header(){
    // if define on linux and mac
    time_t curtime;
    time(&curtime);
    printf("\n\t\t\tVineyard WorkForce Process Manager\n");
    printf("=======================================================================\n");
    printf("  MARTINS Alfredo | HEIOPO | ELTE - Budapest, %s\n", ctime(&curtime));
    printf("   :::::::   ::::::   :::::::   :::::::   :::::::   :::::::   :::::::\n");
    printf("\n");
}

/* === GOODBYE MESSAGE AND RECOGNITION === */
void
dismiss(){
    time_t curtime;
    time(&curtime);
    printf("\nTHANK YOU FOR USING MY SYSTEM :) %s\n", ctime(&curtime));
    printf("Copyright© Operating Systems - ELTE 2023 Spring, Alfredo Martins (Student) & Dr. Bakonyi Viktória (Professor)\n");
}

/* === INSERT APPLICANT === */
void  
add(){
    printf("\n::::::: INSERT APPLICANT :::::::\n");
    printf("Enter a valid name (maximum of %d digits or characters): ", MAX_LEN - 2);
    char str_name[MAX_LEN];
    char c;
    size_t index = 0;
    while (index < MAX_LEN - 1 && ((c = getchar()) != '\n')) {
        str_name[index++] = c;
    }

    str_name[index] = '\0';
    
    printf("Enter the days you would like to work: ");

    char str_days[MAX_LEN];
    index = 0;
    while (index < MAX_LEN - 1  && ((c = getchar()) != '\n')) {
        str_days[index++] = c;
    }

    str_days[index] = '\0';

    // Get the first token
    const char s[2] = " "; // Delimiter
    char * token; // Token to get each string
    token = strtok(str_days, s);

    int cont = 0;

    applicant_t ap;
    strcpy(ap.name, str_name);

    // Walk through other tokens
    bool abort = false;
    while (token != NULL) {
        cont++;
        // printf("%s\n", token);

        if (!is_valid_day(token)) { // Checking whether or not all the days of the week are valid.
            printf("Error checking the text.\n");
            return;
        } else
            if (is_full(token)) {
                abort = true;
                break;
            }

        int len = strlen(token) + 1;
        strcpy(ap.days[cont - 1], token);

        token = strtok(NULL, s);
    }

    ap.choices = cont;

    if(!insert(ap) || abort) {
        if (abort) {
            printf("Sorry :(. This day (\"%s \") is already full.", token);
        } else {
            printf("\nError inserting %s : %s. Please, check info applicant.\n", str_name, str_days); 
        }
    } else{
        printf("\nApplicant inserted sucessfully!\n");
    }
}

/* === REMOVE APPLICANT === */
void  
remove_applicant(){
    printf("\n::::::: REMOVE APPLICANT :::::::\n");
    printf("Enter applicant's name: ");
    char str_name[MAX_LEN];
    char c;
    size_t index = 0;
    while (index < MAX_LEN - 1 && ((c = getchar()) != '\n')) {
        str_name[index++] = c;
    }

    str_name[index] = '\0';

    printf("\n");
    
    if(!remove_(str_name)) {
        printf("\nError removing %s. Please, check info applicant.\n", str_name);
    } else{
        printf("\nApplicant removed sucessfully!\n");
    }
}

/* === MODIFY APPLICANT === */
void
modify_applicant() {
    printf("\n::::::: MODIFY APPLICANT :::::::\n");
    printf("Enter applicant's name: ");
    char str_name[MAX_LEN];
    char c;
    size_t index = 0;
    while (index < MAX_LEN - 1 && ((c = getchar()) != '\n')) {
        str_name[index++] = c;
    }

    str_name[index] = '\0';
    
    printf("Enter new applicant's (maximum of %d digits or characters): ", MAX_LEN - 2);
    char str_new_name[MAX_LEN];
    index = 0;
    while (index < MAX_LEN - 1 && ((c = getchar()) != '\n')) {
        str_new_name[index++] = c;
    }

    str_new_name[index] = '\0';
    
    printf("Enter the days you would like to work: ");

    char str_days[MAX_LEN];
    index = 0;
    while (index < MAX_LEN - 1  && ((c = getchar()) != '\n')) {
        str_days[index++] = c;
    }

    str_days[index] = '\0';

    // Get the first token
    const char s[2] = " "; // Delimiter
    char * token; // Token to get each string
    token = strtok(str_days, s);

    int cont = 0;

    applicant_t ap;
    strcpy(ap.name, str_new_name);

    // Walk through other tokens
    bool abort = false;
    while (token != NULL) {
        cont++;

        if (!is_valid_day(token)) { // Checking whether or not all the days of the week are valid.
            printf("Error checking the text.\n");
            return;
        } else
            if (is_full(token)) {
                abort = true;
                break;
            }

        int len = strlen(token) + 1;
        strcpy(ap.days[cont - 1], token);

        token = strtok(NULL, s);
    }

    ap.choices = cont;

    if(!modify(str_name, ap) || abort) {
        if (abort) {
            printf("Sorry :(. This day (\"%s \") is already full.", token);
        } else {
            printf("\nError modifying %s : %s. Please, check info applicant.\n", str_name, str_days); 
        }
    } else{
        printf("\nApplicant's data modified sucessfully!\n");
    }
}

bool
is_valid_day(char * s) {
    int len = sizeof(DAYS_OF_THE_WEEKS)/sizeof(DAYS_OF_THE_WEEKS[0]);
    for (int i = 0; i < len; ++i) {
        if (strcmp(s, DAYS_OF_THE_WEEKS[i]) == 0) {
            return true;
        }
    } return false;
}

/* === LIST ALL APPLICANTS BY DAYS === */
void
list_applicants_by_days(){
    printf("\n::::::: LIST ALL APPLICANTS BY DAYS :::::::\n\n");

    // list();
    
    read_info_from_file();
    int len = sizeof(DAYS_OF_THE_WEEKS)/sizeof(DAYS_OF_THE_WEEKS[0]);
    for (int i = 0; i < len; ++i) {
        print_day_available(i);
    }
}