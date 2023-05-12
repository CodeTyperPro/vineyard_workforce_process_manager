#include "vineyard.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h> // open, create
#include <signal.h>
#include <sys/types.h> // open
#include <sys/stat.h> // S_IRUSR
#include <unistd.h> // low level_api

#include <time.h>
#include <errno.h> // perror, errno
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#include <string.h>
#include <stdbool.h>
#include <ctype.h>

void 
init_buses(bus_t buses[]) {
    for (int i = 0; i < NUM_BUSES; i++) {
        buses[i].num_workers = 0;
    }
}

int
start_buses() {
    read_info_from_file();

    int len = sizeof(DAYS_OF_THE_WEEKS)/sizeof(DAYS_OF_THE_WEEKS[0]);
    printf("\nDay %d / %d\n", (CURRENT_DAY + 1), len);
    bus_t buses[NUM_BUSES];
    init_buses(buses);


    for (int i = 0; i < all_workers.capacity[CURRENT_DAY]; i++) {
        add_to_bus(buses, all_workers.workers[CURRENT_DAY][i]);
    }

    CURRENT_DAY = (CURRENT_DAY + 1) % len; // 0 1 2 3 4 | 0 1 2 3 4 | 0 1 2 3 4 | ...

    for (int x = 0; x < 2; ++x) {
        // ==== PROCESS BUSES ==== //
        signal(SIGTERM, handler);

        int pipe_fd[2]; // Unnamed pipe file descriptor array

        if (pipe(pipe_fd) == - 1) {
            printf("An error ocurred with opening the pipe.\n");
            exit(EXIT_FAILURE);
        }

        int messg, status;
        key_t key;
        key = ftok(PROGRAM_NAME, 1);
        printf("Key: %d\n", key);
        messg = msgget(key, 0600 | IPC_CREAT);

        msg_t mss;

        if ( messg < 0) {
            perror("msgget error!");
            return 1;
        }


        struct sigaction action;
        action.sa_handler = handler;
        action.sa_flags = SA_RESTART;
        sigaction(SIGUSR1, &action, NULL);

        if (is_empty_bus(buses[x])) {
            continue;
        }

        pid_t child = fork();
        if (child < 0) {
            perror("Unsucessful fork call.\n"); exit(1);
        } 
        
        if (child > 0) {
            // Parent process
            printf("Parent starts and wait ...\n");

            pause(); // Wait for SIGUSR1 from the child arrives.

            close(pipe_fd[0]);
            write(pipe_fd[1], &buses[x], sizeof(buses[x]));
            close(pipe_fd[1]);

            wait(NULL);
            
            // === Receive message starts === //
            int status;
            status = msgrcv(messg, &mss, MAX_TEXT, MSG_TYPE, IPC_NOWAIT);

            if (status < 0) {
                perror("msgrcv error!");
                exit(1);
            } else {
                printf("\nSummary message: \n");
                printf("\tNumber of workers brought in => %s\n", mss.msg_counter_workers);
            }

            status = msgctl(messg, IPC_RMID, NULL);
            if (status < 0) {
                perror("msgctl error!\n");
                return 1;
            }

            // === Receive message ends
            printf("\nParent ended.\n");
        } else {
            sleep(2);
            kill(getppid(), SIGUSR1);
            sleep(2);

            close(pipe_fd[1]);
            read(pipe_fd[0], &buses, sizeof(buses));
            
            printf("\tChild read the message: \n");
            int count_workers = 0;
            
            printf("\t\tBus %d arrived with %d workers: \n", (x + 1), buses[x].num_workers);
            print_workers(buses[x]);
            count_workers += buses[x].num_workers;

            close(pipe_fd[0]);
            sprintf(mss.msg_counter_workers, "%d", count_workers);
            mss.msg_type = MSG_TYPE;

            // === Send message starts === //
            int status;
            int len = strlen(mss.msg_counter_workers) + 1;
            status = msgsnd(messg, &mss, len, IPC_NOWAIT);

            if (status < 0) {
                perror("msgnd error!");
                exit(1);
            }

            // === Send message ends === //

            sleep(2);
            printf("\tChild ended.\n");
            exit(EXIT_SUCCESS);
        }            
    }

    return 1;
}

void
handler(int signumber) {
    printf("Signal from the minibuses: Bus has arrived.\n");
}

bool
bus_has_space(bus_t bus) {
    return bus.num_workers < MAX_CAP;
}

bool
is_empty_bus(bus_t bus) {
    return bus.num_workers == 0;
}

bool
are_equal(applicant_t worker1, applicant_t worker2) {
    return strcmp(worker1.name, worker2.name) == 0;
}

bool
contains_worker(bus_t bus, applicant_t worker) {
    for (int i = 0; i < bus.num_workers; ++i) {
        if (are_equal(bus.workers[i], worker)) {
            return true;
        }
    }
    return false;
}

void
add_to_bus(bus_t buses[], applicant_t worker) {
    for (int i = 0; i < NUM_BUSES; i++) {
        if (bus_has_space(buses[i]) && !contains_worker(buses[i], worker)) {
            buses[i].workers[buses[i].num_workers] = worker;
            buses[i].num_workers++;
            // printf("Hi\n");
            break;
        }
    }
}

void
init_all_workers() {
    for (int i = 0; i < 5; i++) {
        all_workers.capacity[i] = 0;
    }
}

void 
add_to_all_workers(applicant_t app, int index_day) {
    //if (all_workers.capacity[index_day] < MAX_NUM_WORKERS) {
        int pos = all_workers.capacity[index_day];
        all_workers.workers[index_day][pos] = app;
        all_workers.capacity[index_day]++;
    //}
}

int
read_info_from_file() {
    init_all_workers();

    FILE * file;
    file = fopen(FILE_NAME, "r+");

    if (file == NULL) {
        perror("Error opening the file.\n");
        return 0;
    }

    int len = sizeof(DAYS_OF_THE_WEEKS)/sizeof(DAYS_OF_THE_WEEKS[0]);
    applicant_t app;
    size_t num_read; 

    while ((num_read = fread(&app, sizeof(app), 1, file)) == 1) {
        if (ferror(file) || feof(file)) {
            fprintf(stderr, "Error: expected to read %lu bytes, but read %zd bytes.\n", sizeof(applicant_t), num_read);
            break;
        }
            
        // printf("\n%s - %d\n", app.name, app.choices);
        //if (!isalpha(app.name[0])) break;
        
        for (int j = 0; j < app.choices; ++j) {
            bool same_day = false;
            for (int i = 0; !same_day && i < len; ++i) {
                if (strcmp(app.days[j], DAYS_OF_THE_WEEKS[i]) == 0) {
                    // printf("\n%s - %s\n", app.days[j], DAYS_OF_THE_WEEKS[i]);
                    same_day = true;
                    add_to_all_workers(app, i);
                }
            }
        }
        
    }

    fclose(file);

    return 1;
}

void
print_workers(bus_t bus) {
    for (int i = 0; i < bus.num_workers; i++) {
        printf("\t\t\t[%d] - %s\n", (i + 1), bus.workers[i].name);
    } printf("\n");
}

void
print_day_available(int index_day) {
    printf("%s\n", DAYS_OF_THE_WEEKS[index_day]);
    if (all_workers.capacity[index_day] == 0) {
        printf("\tEmpty!\n");
    } else {
        // printf("%d\n", all_workers.capacity[index_day]);
        for (int i = 0; i < all_workers.capacity[index_day]; i++) {
            printf("\t[%d] - %s\n", (i + 1), all_workers.workers[index_day][i].name);
        }
    }

    printf("\n");  
}

bool
insert(applicant_t app){
    FILE * file;

    // 0644 => create the file with the specific permission
    file = fopen(FILE_NAME, "a+");

    if (file == NULL) {
        perror("Error opening the file.\n");
        return false;
    }

    fwrite(&app, sizeof(app), 1, file);
    // printf("One time\n");
    if (ferror(file)) {
        perror("Error writting in the file.\n");
        return false;
    }

    //rewind(file);
    fclose(file);

    return true;
}

bool
modify(char * str_name, applicant_t app){
    FILE * file;
    // 0644 => create the file with the specific permission

    if (access(FILE_NAME, F_OK) != 0) {
        // File does not exist, create it
        file = fopen(FILE_NAME, "r+");
    } else {
        // File exists, open it
        file = fopen(FILE_NAME, "r");
    }

    if (file == NULL) {
        perror("Error opening the file.\n");
        return false;
    }

    applicant_t applicants[MAX_NUMBER_APPLICANTS];
    int index = 0, index_applicant = -1;

    applicant_t app_cur;
    while (index < MAX_NUMBER_APPLICANTS && (fread(&app_cur, sizeof(app_cur), 1, file)) == 1) {
        applicants[index] = app_cur;

        // if (!isalpha(app_cur.name[0])) break;

        if (strcmp(app_cur.name, str_name) == 0) { // Found the applicant to be modified
            index_applicant = index;
        }


        index++;
    }
    
    fclose(file);

    if (index_applicant == -1) {
        printf("Applicant not found.\n");
        return false;
    }

    applicants[index_applicant] = app;

    // Writing back
    file = fopen(FILE_NAME, "w+");

    if (file == NULL) {
        perror("Error opening the file.\n");
        return false;
    }    

    for (int i = 0; i < index; ++i) {
        int write_res = fwrite(&applicants[i], sizeof(applicants[i]), 1, file);
        if (write_res != 1) {
            perror("Error writting in the file.\n");
            return false;
        } 
    }

    fclose(file);

    return true;
}

bool
remove_(char str[]) {
    applicant_t applicants[MAX_NUMBER_APPLICANTS];
    int index = 0, index_applicant = -1;

    FILE * file = fopen(FILE_NAME, "r");

    if (file == NULL) {
        perror("Error opening the file.\n");
        return false;
    }

    applicant_t app;
    while (fread(&app, sizeof(app), 1, file) == 1) {

        if (ferror(file) || feof(file)) {
            printf("Error!");
            break;
        }
        
        // if (!isalpha(app.name[0])) break;

        applicants[index] = app;

        if (index_applicant == -1 && strcmp(applicants[index].name, str) == 0) { // Found the applicant to be removed
            index_applicant = index;
        }

        index++;
    }
    
    fclose(file);

    if (index_applicant == -1) {
        printf("Applicant not found.\n");
        return false;
    }

    // Writing back
    // remove(FILE_NAME);
    init_all_workers();
    
    file = fopen(FILE_NAME, "w");

    if (file == NULL) {
        perror("Error opening the file.\n");
        return false;
    }

    // printf("Index: %d\nIndex applicant: %d\n", index, index_applicant);
    for (int i = 0; i < index; ++i) {

        if (i == index_applicant) continue;

        fwrite(&applicants[i], sizeof(applicants[i]), 1, file);
        if (ferror(file)) {
            perror("Error writting in the file.\n");
            return false;
        }
    }

    fclose(file);

    return true;
}

void
list() {
    FILE * file = fopen(FILE_NAME, "r");

    if (file == NULL) {
        perror("Error opening the file.\n");
        return;
    }

    applicant_t app;
    size_t num_read;
    while ((num_read = fread(&app, sizeof(app), 1, file)) == 1) {
        if (ferror(file) || feof(file)) {
            fprintf(stderr, "Error: expected to read %lu bytes, but read %zd bytes.\n", sizeof(app), num_read);
            break;
        }

        print_applicant(app);
        printf("\n");
    } 

    if (num_read == -1) {
        perror("Error reading from file.");
    }
    
    fclose(file);
}

void
print_applicant(applicant_t app) {
    printf("\t%s => ", app.name);
    
    for (int i = 0; i < app.choices; ++i) {
        printf("%s ", app.days[i]);
    }
}

bool
is_full (char * day) {
    FILE * file = fopen(FILE_NAME, "r+");

    if (file == NULL) {
        return false;
    }

    applicant_t app;
    size_t num_read;

    int count_applicants = 0;
    while ((num_read = fread(&app, sizeof(app), 1, file)) == 1) {

        for (int i = 0; i < app.choices; ++i) {
            if (strcmp(app.days[i], day) == 0) {
                count_applicants++;
            }
        }
    }

    fclose(file);

    int at = at_day(day);
    if (at == -1) {
        printf("Error in the content file.\n");
        return false;
    }

    return !(count_applicants < LIMIT_APPLICANTS_PER_DAY[at]);
}

int
at_day(char * str) {
    int len = sizeof(DAYS_OF_THE_WEEKS)/sizeof(DAYS_OF_THE_WEEKS[0]);
    for (int i = 0; i < len; ++i) {
        if(strcmp(DAYS_OF_THE_WEEKS[i], str) == 0) {
            return i;
        }
    } return -1;
}
