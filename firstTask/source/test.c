#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define ASSERT(CONDITION)               \
    if(!CONDITION) {                    \
        printf("Hello, Ama Assert");    \
        exit(0);                        \
    }

int main(int argc, char** argv) {
    if (argc == 2) {
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

