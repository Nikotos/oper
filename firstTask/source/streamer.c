#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


//------------------DEFINE BUSINESS---------------------------------------------------
#define OAK_DEBUG

#ifdef OAK_DEBUG
#define PRINTF printf
#else
#define PRINTF
#endif

#define EXIT                                \
    do {                                    \
        printf("Exiting the programm!\n");  \
        exit(0);                            \
    } while(0)

#define SAFETY_OPEN_FIFO(FIFO_VAR_NAME, FIFO_WAY, FLAGS)            \
    do {                                                            \
        FIFO_VAR_NAME = open(FIFO_WAY, FLAGS);                      \
        if (FIFO_VAR_NAME == -1) {                                  \
            printf("Cant open fifo!\n");                            \
            return -1;                                              \
        }                                                           \
    }   while(0)
//------------------------------------------------------------------------------------

//------------------CONFIG------------------------
#define TIMEOUT 10
#define BUFFER_SIZE 1024
#define FIFO_PATH "../fifo"
char globalStaticBuffer[BUFFER_SIZE];
//------------------------------------------------



int continuoslyWriteFromFileToPipe(int inputFileDescriptor, int pipeFileDescriptor) {
    if (pipeFileDescriptor == -1) {
        printf("Pipe wasn't open!\n");
        return 1;
    }
    if (inputFileDescriptor == -1) {
        printf("Problems with input file!");
        return 1;
    }
    int writtenDataAmount = 0;
    while ((writtenDataAmount = read(inputFileDescriptor, globalStaticBuffer, BUFFER_SIZE)) &&  writtenDataAmount > 0) {
        write(pipeFileDescriptor, globalStaticBuffer, writtenDataAmount);
    }
    return 0;
}

int establishedConnectionDataFifoWriteSide(int connectionSettingPipe) {
    int pid = -1;
    char dataFifoFullName[50] = {0};
    read(connectionSettingPipe, &pid, sizeof(int));
    sprintf(dataFifoFullName, "../dataFifo%d", pid);
    int dataFifoDescriptor = open(dataFifoFullName, O_WRONLY | O_NONBLOCK);
    if ((dataFifoDescriptor == -1) && (errno == ENXIO)) {
        printf("Follower had died!");
        EXIT
    }
    fcntl(dataFifoDescriptor, F_SETFL, O_WRONLY);
    unlink(dataFifoFullName);
    return dataFifoDescriptor;
}

int main(int argc, char** argv) {
    if (argc == 2) {
        //-------------------------
        int connectionSettingPipe;
        int inputFileDescriptor;
        //-------------------------
        PRINTF("Waiting for follower\n");
        SAFETY_OPEN_FIFO(connectionSettingPipe, FIFO_PATH, O_RDWR);
        int dataFifoDescriptor = establishedConnectionDataFifoWriteSide(connectionSettingPipe);
        SAFETY_OPEN_FIFO(inputFileDescriptor, argv[1], O_RDONLY)
        continuoslyWriteFromFileToPipe(inputFileDescriptor, dataFifoDescriptor);
        PRINTF("Got it!\n");
    } else {
        printf("Inappropriate amount of arguments!");
        return 1;
    }
    return 0;
}

