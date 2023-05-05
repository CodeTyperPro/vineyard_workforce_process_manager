#ifndef VINEYARD_H
#define VINEYARD_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_LEN 102
#define MAX_SIZE 7
#define MAX_NUMBER_APPLICANTS 5000 // 

const char * DAYS_OF_THE_WEEKS [] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
char * FILE_NAME;
int LIMIT_APPLICANTS_PER_DAY [] = {10, 5, 10, 5, 10};

typedef struct applicant {
    char name[MAX_LEN];
    char days[MAX_SIZE][MAX_LEN];
    int  choices;
} applicant_t;

bool        insert(applicant_t app);
bool        modify(char * str_name, applicant_t app);
bool        remove_(char str[]);
void        list();
void        list_by_day(const char * day);
void        print_applicant(applicant_t app);
bool        is_full(char * day);
int         at_day(char * str);

#endif