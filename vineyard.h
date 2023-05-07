#ifndef VINEYARD_H
#define VINEYARD_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LEN 102
#define MAX_SIZE 7
#define MAX_NUMBER_APPLICANTS 5000 // 
#define MAX_LEN 102
#define MAX_CAP 5
#define MAX_SIZE 7
#define MAX_NUMBER_workerS 5000
#define MAX_CAP_PER_DAY 10
#define NUM_BUSES 2
#define MSG_TYPE 5
#define MAX_NUM_WORKERS 5
#define MAX_DAY_WORK 10
#define MAX_TEXT 1024

const char * DAYS_OF_THE_WEEKS [] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
char FILE_NAME[MAX_LEN], PROGRAM_NAME[MAX_LEN];
int LIMIT_APPLICANTS_PER_DAY [] = {MAX_DAY_WORK, MAX_DAY_WORK, MAX_DAY_WORK, MAX_DAY_WORK, MAX_DAY_WORK};
int CURRENT_DAY = 0;

typedef struct applicant {
    char name[MAX_LEN];
    char days[MAX_SIZE][MAX_LEN];
    int  choices;
} applicant_t;

struct workers{
    int capacity[5];
    applicant_t workers[5][MAX_NUM_WORKERS];
};

struct workers all_workers;

typedef struct {
    int num_workers;
    applicant_t workers[MAX_CAP]; 
} bus_t;

typedef struct {
    long msg_type;
    char msg_counter_workers[MAX_TEXT];
} msg_t;

bool    insert(applicant_t app);
bool    modify(char * str_name, applicant_t app);
bool    remove_(char str[]);
void    list();
void    print_applicant(applicant_t app);
bool    is_full(char * day);
int     at_day(char * str);
int     start_buses();

int     msgget(key_t key, int msgflg);
int     send(int msg_queue, msg_t * msg);
int     receive(int msg_queue);

void    handler(int signumber);
bool    bus_has_space(bus_t bus);
bool    is_empty_bus(bus_t bus);
bool    are_equal(applicant_t worker1, applicant_t worker2);
bool    contains_worker(bus_t bus, applicant_t worker);
void    add_to_bus(bus_t buses[], applicant_t worker);
void    init_all_workers();
void    add_to_all_workers(applicant_t worker, int index_day);
int     read_info_from_file();
void    print_workers(bus_t bus);
void    print_day_available(int index_day);


#endif