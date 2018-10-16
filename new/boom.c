#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>



//--------DFINE BUSINESS--------------------------------------------------
#define CHECK_CALL(MESSAGE, EXPRESSION)          \
    do {                                         \
        if (!(EXPRESSION)) {                     \
            perror("Message = '"#MESSAGE"'; "    \
            "expression = ("#EXPRESSION")");     \
            exit(EXIT_FAILURE);                  \
        }                                        \
    } while(0)

//------------------------------------------------------------------------


//------------------CONFIG------------------------
#define MAX_PROCESSES_AMOUNT 1245
#define PERMISSIONS 0644
//------------------------------------------------

long int getPositiveNumberFromInput(char** argv) {
    char* endptr = NULL;
    long int number = strtol(argv[0], &endptr, 0);
    if (number == 0) {
        printf("Inappropriate Symbol!\n");
        return -1;
    }
    else if (*endptr != 0) {
        printf("Inappropriate Symbol in the end - [%c]\n", *endptr);
        return -1;
    }
    else if (errno == ERANGE) {
        printf("Number out of range!\n");
        return -1;
    }
    else if (number < 0) {
        printf("Number is abive ZERO!!!");
        return -1;
    }
    else {
        return number;
    }
}



int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Inappropriate amount of agruments!!!\n");
    } else {
        int amountOfProcesses = getPositiveNumberFromInput(argv + 1);
        int increasmentAmount = getPositiveNumberFromInput(argv + 2);
        if (amountOfProcesses < MAX_PROCESSES_AMOUNT) {
            
            int sharedMemoryID = shmget(IPC_PRIVATE, sizeof(int), PERMISSIONS);
            CHECK_CALL(shmget, sharedMemoryID != -1);
            int* poorTrickyVariable = shmat(sharedMemoryID, NULL, PERMISSIONS);
            

            int semaphoresID = semget(IPC_PRIVATE,  amountOfProcesses, PERMISSIONS);
            
            struct sembuf semops[amountOfProcesses];
            for(int i = 0; i < amountOfProcesses; i++) {
                semops[i].sem_num = i;
                semops[i].sem_op = 1;
                semops[i].sem_flg = 0;
            }
            semop(semaphoresID, &semops, amountOfProcesses);
            pid_t pid = 0;
            for(int i = 0; i < amountOfProcesses; i++) {
                pid = fork();
                CHECK_CALL(fork, pid != -1);
                if (pid == 0) {
                    for(int j = 0; j < increasmentAmount; i++) {
                        //-----CRITICAL SECTION-----------------------------------------------
                        struct sembuf semaphoreOperation;
                        semaphoreOperation.sem_num = j;
                        semaphoreOperation.sem_op = 0;
                        semop(semaphoresID, &semaphoreOperation, amountOfProcesses);
                        (*poorTrickyVariable) += 1;
                        if (j + 1 != amountOfProcesses) {
                            semaphoreOperation.sem_num = j + 1;
                            semaphoreOperation.sem_op = -1;
                            semop(semaphoresID, &semaphoreOperation, amountOfProcesses);
                        }
                        //--------------------------------------------------------------------
                    }
                    exit(EXIT_SUCCESS);
                }
            }
            
            struct sembuf semaphoreOperation;
            semaphoreOperation.sem_num = 0;
            semaphoreOperation.sem_op = -1;
            semop(semaphoresID, &semaphoreOperation, amountOfProcesses);
            
            for(int i = 0; i < amountOfProcesses; i++) {
                wait(NULL);
            }
            
            printf("expected - [%d]\n", increasmentAmount * amountOfProcesses);
            printf("We got - [%d]\n", *poorTrickyVariable);
            
            shmdt(poorTrickyVariable);
            
        } else {
            printf("You wanna make two many processes!\n");
        }
        
    }
}








