#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>


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


int globalMutableVariable = 0;

void *trickyFunction(void* argv) {
    int amountOfIncreases = (int*)argv;
    for (int i = 0; i < amountOfIncreases; i++) {
        globalMutableVariable++;
    }
    return NULL;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Inappropriate amount of agruments!!!\n");
    } else {
        int n = getPositiveNumberFromInput(argv + 1);
        int k = getPositiveNumberFromInput(argv + 2);
        
        if (n < 10000) {
            pthread_t threadId[10000];
            for (int i = 0; i < n; i++) {
                pthread_create(threadId + i, NULL, trickyFunction, k);
            }
            for (int i = 0; i < n; i++) {
                pthread_join(*(threadId + i), NULL);
            }
            printf("%d\n", globalMutableVariable);
        }
        
    }
}








