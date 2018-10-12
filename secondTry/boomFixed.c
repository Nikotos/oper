#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


long int getPositiveNumberFromInput(int argc, char** argv) {
    if (argc > 1) {
        char* endptr = NULL;
        long int number = strtol(argv[1], &endptr, 0);
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
}


int main(int argc, char** argv) {
    long int number = getPositiveNumberFromInput(argc, argv);
    if (number == -1) {
        return 0;
    }
    else {
        if (number < 1245) {
            int forkResult = 0;
            for(int i = 0; i < number; i++){
                forkResult = fork();
                if (forkResult == 0) {
                    //child process
                    printf("number - [%d]\nPID is - [%d]\n", i, getpid());
                    return 0;
                } else {
                    //parent process
                    waitpid(forkResult, NULL, 0);
                    printf("killed - [%d]\n\n", forkResult);
                }
            }
        }
        else {
            printf("You Wanna make tooo many processes!\n");
        }
    }
    return 0;
}
