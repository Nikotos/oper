#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>


#define BUFFER_SIZE 1024 //I mean symbols

int main(int argc, char** argv) {
    if (argc == 2) {
        //----------------------------
        char* filename = argv[1];
        pid_t pid;
        int fd[2];
        char buffer[BUFFER_SIZE];
        //-----------------------------
        if (pipe(fd) != 0) {
            printf ("Pipe failed.\n");
            return 1;
        }
        pid = fork();
        if (pid == 0) {
            //child process
            close(fd[0]);   //configurating pipe, closing reading side of pipe
            int myFileDescriptor = open(filename, O_RDONLY); //open file
            if (myFileDescriptor == -1) {
                printf("Can't find file  [%s]\n", filename);
                return 1;
            }
            int writtenDataAmount = 0;
            while ((writtenDataAmount = read(myFileDescriptor, &buffer, BUFFER_SIZE)) &&  writtenDataAmount > 0) {
                write(fd[1], buffer, writtenDataAmount);
            }
            
            close(fd[1]);
            close(myFileDescriptor);
            
            return 0;
        } else {
            //parent process
            close(fd[1]);  //configurating pipe, closing writing side of pipe
            int readDataAmount = 0;
            
            int outputFileDescriptor = open("output", O_CREAT | O_WRONLY);
            
            while ((readDataAmount = read(fd[0], &buffer, BUFFER_SIZE)) && readDataAmount > 0) {
                write(outputFileDescriptor, &buffer, readDataAmount);
            }
            close(fd[0]);
            close(outputFileDescriptor);
        }
        
        wait(NULL);
        
    } else {
        printf("Inappropriate amount of arguments!");
        return 1;
    }
    return 0;
}
