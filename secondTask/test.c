#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>


//------------------CONFIG------------------------
#define MSG_QUEUE_PERMISSIONS 0600
//------------------------------------------------

//------------------DEFINE BUSINESS---------------------------------------------------
#define OAK_DEBU

#ifdef OAK_DEBUG
#define PRINTF printf
#else
#define PRINTF
#endif


//-----------------------------------------------------------------------------------

struct Message {
    long type;
};

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
    } else {
        printf("Inappropriate Command Line Input!\n");
    }
    return -1;
}



int main(int argc, char** argv) {
    long int number = getPositiveNumberFromInput(argc, argv);
    if (number == -1) {
        return 0;
    }
    else {
        if (number < 1245) {
            int messageQueueID = msgget(IPC_PRIVATE, MSG_QUEUE_PERMISSIONS);
            if (messageQueueID == -1) {
                printf("Error during creatig queue\n");
                return 0;
            }
            pid_t result = 0;
            pid_t i;
            
            struct Message msg = { 0 };
            
            msg.type = 1;
            int sendResult = msgsnd(messageQueueID, &msg, 0, 0);
            
            for(i = 1; i <= number; i++) {
                result = fork();
                if (result == 0) {
                    //-----Child process------------------------------------------------
                    msg.type = 0;
                    int recieveResult = msgrcv(messageQueueID, &msg, 0, i, 0);
                    printf("%d ", i);
                    fflush(stdout);
                    
                    msg.type = i + 1;
                    msgsnd(messageQueueID, &msg, 0, 0);
                    return 0;
                    //-----------------------------------------------------------------------
                }
            }
            
            msg.type = 1;
            int sendResult = msgsnd(messageQueueID, &msg, 0, 0);
            
            msg.type = 0;
            int recieveResult = msgrcv(messageQueueID, &msg, 0, number, 0);
            
            msgctl(messageQueueID, IPC_RMID, NULL);
            usleep(100);   // For stress test output looks normally
        }
        else {
            printf("You Wanna make tooo many processes!\n");
        }
    }
    return 0;
}
