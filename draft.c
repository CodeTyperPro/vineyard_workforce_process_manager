#include <fcntl.h>
#include <sys/types.h>
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
#define MAX_CAP 5
#define MAX_SIZE 7
#define MAX_NUMBER_workerS 5000
#define MAX_CAP_PER_DAY 10
#define NUM_BUSES 2
#define MSG_TYPE 5

const char * DAYS_OF_THE_WEEKS [] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
char FILE_NAME[MAX_LEN];

typedef struct {
    char name[MAX_LEN];
    char days[MAX_SIZE][MAX_LEN];
    int  choices;
} worker_t;

typedef struct {
    int num_workers;
    worker_t workers[MAX_CAP]; 
} bus_t;

typedef struct {
    long mtype;
    int counter_worker;
} msg_t;

int msgget(key_t key, int msgflg);

int send(int msg_queue, const msg_t * msg) {
    int status;
    status = msgsnd(msg_queue, msg, sizeof(msg->counter_worker), IPC_NOWAIT);

    if (status < 0) {
        perror("msgnd error!");
    }

    return 0;
}

int receive(int msg_queue) {
    msg_t msg;
    int status;

    status = msgrcv(msg_queue, &msg, 1024, MSG_TYPE, 0);

    if (status < 0) {
        perror("msgrcv error!");
    } else {
        printf("Summary message: \n");
        printf("%d of the requested workers have been brought in.\n", msg.counter_worker);
        printf("Number of workers brougth in => %d\n", msg.counter_worker);
    }

    return 0;
}

void
list_by_day(const char * day);

void handler(int signumber) {
    printf("Signal with number %i has arrived\n", signumber);
}

bool bus_has_space(bus_t bus) {
    return bus.num_workers < MAX_CAP;
}

bool is_empty_bus(bus_t bus) {
    return bus.num_workers == 0;
}

bool are_equal(worker_t worker1, worker_t worker2) {
    return strcmp(worker1.name, worker2.name) == 0;
}

bool contains_worker(bus_t bus, worker_t worker) {
    for (int i = 0; i < bus.num_workers; ++i) {
        if (are_equal(bus.workers[i], worker)) {
            return true;
        }
    }
    return false;
}

void add_to_bus(bus_t buses[], worker_t worker) {
    for (int i = 0; i < NUM_BUSES; i++) {
        if (bus_has_space(buses[i]) && !contains_worker(buses[i], worker)) {
            buses[i].workers[buses[i].num_workers] = worker;
            buses[i].num_workers++;
        }
    }
}

void print_workers(bus_t bus) {
    for (int i = 0; bus.num_workers; i++) {
        printf("[%d] - %s\n", (i + 1), bus.workers[i].name);
    } printf("\n");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        perror("Sorry ): ... Invalid number of arguments. 2 arguments were expected. Please, try again later. Thank you :)!\n");
        exit(1); // Ends the program.
    }

    strcpy(FILE_NAME, argv[1]);
    bus_t buses[NUM_BUSES];

    int file = open(FILE_NAME, O_RDONLY | S_IRUSR);

    if (file < 0) {
        perror("Error opening the file.\n");
        return 0;
    }

    int len = sizeof(DAYS_OF_THE_WEEKS)/sizeof(DAYS_OF_THE_WEEKS[0]);
    for (int i = 0; i < len; ++i) {
        printf("%s:\n", DAYS_OF_THE_WEEKS[i]);
        worker_t worker;
        ssize_t num_read;

        while ((num_read = read(file, &worker, sizeof(worker_t))) > 0) {
            
            if (num_read != sizeof(worker_t)) {
                fprintf(stderr, "Error: expected to read %lu bytes, but read %zd bytes.\n", sizeof(worker_t), num_read);
                break;
            }

            bool same_day = false;
            for (int i = 0; !same_day && i < worker.choices; ++i) {
                if (strcmp(worker.days[i], DAYS_OF_THE_WEEKS[i]) == 0) {
                    same_day = true;
                }
            }

            if (same_day) {
                printf("%s\n", worker.name);
                add_to_bus(buses, worker);
            }
        } 

        // ==== PROCESS BUSES ==== //
        signal(SIGTERM, handler);

        int pipe_fd[2]; // Unnamed pipe file descriptor array

        if (pipe(pipe_fd) == - 1) {
            printf("An error ocurred with opening the pipe.\n");
            exit(EXIT_FAILURE);
        }

        int messg, status;
        key_t key;
        key = ftok(argv[0], 1);
        printf("Key: %d\n", key);
        messg = msgget(key, 0600 | IPC_CREAT);

        if ( messg < 0) {
            perror("msgget error!");
            return 1;
        }

        pid_t child = fork();
        if (child < 0) {
            perror("Unsucessful fork call.\n"); exit(1);
        } 
        
        if (child > 0) {
            // Parent process
            printf("Parent starts!\n");

            close(pipe_fd[0]);
            write(pipe_fd[1], &buses, sizeof(buses));
            close(pipe_fd[1]);

            wait(NULL);
            
            receive(messg);

            printf("Parent ended.\n");
        } else {
            // Child process
            printf("Buses have arrived.\n");

            close(pipe_fd[1]);
            read(pipe_fd[0], &buses, sizeof(buses));
            
            printf("Child read the message: \n");
            int count_workers = 0;
            for (int i = 0; i < NUM_BUSES; i++) {
                if (is_empty_bus(buses[i])) {
                    printf("Bus %d is arrived empty.\n", (i + 1));
                } else {
                    printf("Bus %d arrived with %d workers: \n", (i + 1), buses[i].num_workers);
                    print_workers(buses[i]);
                    count_workers += buses[i].num_workers;
                }
            }

            close(pipe_fd[0]);

            msg_t mss = {MSG_TYPE, count_workers};
            send(messg, &mss);

            status = msgctl(messg, IPC_RMID, NULL);

            if (status < 0) {
                perror("msgctl error!\n");
            }
            
            sleep(3);
            kill(getppid(), SIGTERM);
            printf("Child ended.\n");
        }


/*      if (num_read == -1) {
            perror("Error reading from file.");
        }*/
        
        close(file);
    }

    return 0;
}