#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

//------------------CONFIG----------------------
#define BUFFER_SIZE 1024 //I mean symbols
#define OUTPUT_FILENAME "output"
char globalStaticBuffer[BUFFER_SIZE];
//----------------------------------------------


//------------------SOME TRICKS----------------------
#define SAFETY_LAUNCH(FUNCTION_CALL_RETURN_VALUE)                \
    if (FUNCTION_CALL_RETURN_VALUE) {                            \
        return 1;                                                \
    }
//---------------------------------------------------



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
        write(outputFileDescriptor, globalStaticBuffer, readDataAmount);
    }
    chmod(OUTPUT_FILENAME, 0000666);
    return 0;
}

void prepareFifo() {
    mkfifo("fifo", 0644);
}


int main(int argc, char** argv) {
    if (argc == 2) {
        //---------------------------
        char* filename = argv[1];
        pid_t forkResult;
        int fifo;
        char buffer[BUFFER_SIZE];
        //-----------------------------
        prepareFifo();
        
        forkResult = fork();
        if (forkResult == 0) {
            SAFETY_LAUNCH(continuoslyWriteFromFileToPipe(open(filename, O_RDONLY), open("fifo", O_WRONLY)))
        } else {
            SAFETY_LAUNCH(continuoslyReadFromPipeToFile(open("fifo", O_RDONLY), open(OUTPUT_FILENAME, O_CREAT | O_WRONLY)));
        }
    } else {
        printf("Inappropriate amount of arguments!");
        return 1;
    }
    return 0;
}
