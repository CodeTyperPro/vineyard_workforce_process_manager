#ifndef UTIL_H
#define UTIL_H

#define SIZE_NAME 10
#define SIZE_PHONE_NUMBER 12

#include "vineyard.h"
#include <stdbool.h>

/* === START POINT === */
void  run();

/* === INTERFACES === */
void  header();
void  dismiss();
void  add();
void  remove_applicant();
void  modidy_applicant();
void  list_applicants();
void  list_applicants_by_days();
void  exit_();

/* === AUXILIARY FUNCTIONS === */;
bool  is_valid_day(char s[]);

#endif