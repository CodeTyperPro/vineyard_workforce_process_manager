#include "vineyard.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h> // open, creat
#include <sys/types.h> // open
#include <sys/stat.h> // S_IRUSR
#include <unistd.h> // low level_api

bool
insert(applicant_t app){
    int file;

    // 0644 => create the file with the specific permission

    if (access(FILE_NAME, F_OK) != 0) {
        // File does not exist, create it
        file = open(FILE_NAME, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    } else {
        // File exists, open it
        file = open(FILE_NAME, O_WRONLY | O_APPEND);
    }

    if (file == -1) {
        perror("Error opening the file.\n");
        return false;
    }

    int write_res = write(file, &app, sizeof(app));
    if (write_res != sizeof(app)) {
        perror("Error writting in the file.\n");
        return false;
    }

    close(file);

    return true;
}

bool
modify(char * str_name, applicant_t app){
    int file;
    // 0644 => create the file with the specific permission

    if (access(FILE_NAME, F_OK) != 0) {
        // File does not exist, create it
        file = open(FILE_NAME, O_RDONLY | O_CREAT | S_IRUSR | S_IWUSR);
    } else {
        // File exists, open it
        file = open(FILE_NAME, O_RDONLY | S_IRUSR | S_IWUSR);
    }

    if (file == -1) {
        perror("Error opening the file.\n");
        return false;
    }

    applicant_t applicants[MAX_NUMBER_APPLICANTS];
    int index = 0, index_applicant = -1;

    applicant_t app_cur;
    while (index < MAX_NUMBER_APPLICANTS && read(file, &app_cur, sizeof(applicant_t))) {
        applicants[index] = app_cur;

        if (strcmp(app_cur.name, str_name) == 0) { // Found the applicant to be modified
            index_applicant = index;
        }

        index++;
    }
    
    close(file);

    if (index_applicant == -1) {
        printf("Applicant not found.\n");
        return false;
    }

    applicants[index_applicant] = app;

    // Writing back
    file = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    if (file < 0) {
        perror("Error opening the file.\n");
        return false;
    }    

    for (int i = 0; i < index; ++i) {
        int write_res = write(file, &applicants[i], sizeof(applicants[i]));
        if (write_res != sizeof(applicants[i])) {
            perror("Error writting in the file.\n");
            return false;
        } 
    }

    close(file);

    return true;
}

bool remove_(char str[]) {
    applicant_t applicants[MAX_NUMBER_APPLICANTS];
    int index = 0, index_applicant = -1;

    int file = open(FILE_NAME, O_RDONLY | S_IRUSR | S_IWUSR);

    if (file < 0) {
        perror("Error opening the file.\n");
        return false;
    }

    applicant_t app;
    while (read(file, &app, sizeof(applicant_t))) {
        applicants[index] = app;

        if (strcmp(applicants[index].name, str) == 0) { // Found the applicant to be removed
            index_applicant = index;
        }

        index++;
    }
    
    close(file);

    if (index_applicant == -1) {
        printf("Applicant not found.\n");
        return false;
    }

    // Writing back
    file = open(FILE_NAME, O_WRONLY | O_TRUNC, S_IRUSR | O_APPEND | S_IWUSR);

    if (file < 0) {
        perror("Error opening the file.\n");
        return false;
    }    

    for (int i = 0; i < index; ++i) {
        if (i == index_applicant) continue;

        int write_res = write(file, &applicants[i], sizeof(applicants[i]));
        if (write_res != sizeof(applicants[i])) {
            perror("Error writting in the file.\n");
            return false;
        } 
    }

    close(file);

    return true;
}

void
list() {
    int file = open(FILE_NAME, O_RDONLY | S_IRUSR);

    if (file < 0) {
        perror("Error opening the file.\n");
        return;
    }

    applicant_t app;
    ssize_t num_read;
    while ((num_read = read(file, &app, sizeof(applicant_t))) > 0) {
        
        if (num_read != sizeof(applicant_t)) {
            fprintf(stderr, "Error: expected to read %lu bytes, but read %zd bytes.\n", sizeof(applicant_t), num_read);
            break;
        }

        print_applicant(app);
        printf("\n");
    } 

    if (num_read == -1) {
        perror("Error reading from file.");
    }
    
    close(file);
}

void
list_by_day(const char * day) {
    int file = open(FILE_NAME, O_RDONLY | S_IRUSR);

    if (file < 0) {
        perror("Error opening the file.\n");
        return;
    }

    applicant_t app;
    ssize_t num_read;
    while ((num_read = read(file, &app, sizeof(applicant_t))) > 0) {
        
        if (num_read != sizeof(applicant_t)) {
            fprintf(stderr, "Error: expected to read %lu bytes, but read %zd bytes.\n", sizeof(applicant_t), num_read);
            break;
        }

        bool same_day = false;
        for (int i = 0; !same_day && i < app.choices; ++i) {
            if (strcmp(app.days[i], day) == 0) {
                same_day = true;
            }
        }

        if (same_day) {
            print_applicant(app);
            printf("\n");
        }
    } 

    if (num_read == -1) {
        perror("Error reading from file.");
    }
    
    close(file);
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
    int file = open(FILE_NAME, O_RDONLY | S_IRUSR);

    if (file < 0) {
        return false;
    }

    applicant_t app;
    ssize_t num_read;

    int count_applicants = 0;
    while ((num_read = read(file, &app, sizeof(applicant_t))) > 0) {
        
        for (int i = 0; i < app.choices; ++i) {
            if (strcmp(app.days[i], day) == 0) {
                count_applicants++;
            }
        }
    } 

    close(file);

    int at = at_day(day);
    if (at == -1) {
        printf("Error in the content file.\n");
        return false;
    }

    return !(count_applicants < LIMIT_APPLICANTS_PER_DAY[at]);
}

int at_day(char * str) {
    int len = sizeof(DAYS_OF_THE_WEEKS)/sizeof(DAYS_OF_THE_WEEKS[0]);
    for (int i = 0; i < len; ++i) {
        if(strcmp(DAYS_OF_THE_WEEKS[i], str) == 0) {
            return i;
        }
    } return -1;
}