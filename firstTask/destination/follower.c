#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <poll.h>



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
#define TIMEOUT 10000               // Milliseconds
#define BUFFER_SIZE 1024
#define FIFO_PATH "../fifo"
char globalStaticBuffer[BUFFER_SIZE];
//------------------------------------------------


int continuoslyReadFromPipeToFile(int pipeFileDescriptor, int outputFileDescriptor) {
    if (pipeFileDescriptor == -1) {
        printf("Pipe wasn't open!\n");
        return 1;
    }
    if (outputFileDescriptor == -1) {
        printf("Problems with output file!");
        return 1;
    }
    int readDataAmount = 0;
    while ((readDataAmount = read(pipeFileDescriptor, &globalStaticBuffer, BUFFER_SIZE)) && readDataAmount > 0) {
        write(outputFileDescriptor, &globalStaticBuffer, readDataAmount);
    }
    return 0;
}

int waitForReaction(int fifoFileDes) {
    struct pollfd fds;
    fds.fd = fifoFileDes;
    fds.events = POLLIN;
    int pollResult = poll(&fds, 1, TIMEOUT);
    if ((pollResult == 0) || (pollResult == -1)) {
        printf("Time limit for waiting streamer Exceeded! Or Error has been ocused!\n");
        EXIT;
    } else if ((pollResult == 1) && (fds.revents == POLLIN)) {
        PRINTF("Streamer had succesfully connected!\n");
        return 0;
    }   
    EXIT;
    return 1;
}


int establishedConnectionDataFifoReadSide(int connectionSettingPipe) {
    int pid = getpid();
    char dataFifoFullName[50] = {0};
    sprintf(dataFifoFullName, "../dataFifo%d", pid);
    mkfifo(dataFifoFullName, 0644);
    int dataFifoDescriptor = open(dataFifoFullName, O_RDONLY | O_NONBLOCK);
    PRINTF("Sended PID to pipe, waiting for streamer!\n");
    write(connectionSettingPipe, &pid, sizeof(int));
    waitForReaction(dataFifoDescriptor);
    fcntl(dataFifoDescriptor, F_SETFL, O_RDONLY);
    return dataFifoDescriptor;
}


int main(int argc, char** argv) {
    if (argc == 2) {
        //-------------------------
        int connectionSettingPipe;
        int outputFileDescriptor;
        //-------------------------
        SAFETY_OPEN_FIFO(connectionSettingPipe, FIFO_PATH, O_RDWR);
        int dataFifoDescriptor = establishedConnectionDataFifoReadSide(connectionSettingPipe);
        SAFETY_OPEN_FIFO(outputFileDescriptor, argv[1], O_CREAT | O_WRONLY);
        continuoslyReadFromPipeToFile(dataFifoDescriptor, outputFileDescriptor);
        chmod(argv[1], 0000666);
        PRINTF("Got it!\n");
    } else {
        printf("Inappropriate amount of arguments!");
        return 1;
    }
    return 0;
}


