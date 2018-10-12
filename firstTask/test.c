#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


int fileDescriptorsInternalsComparator(int firstFD, int secondFD) {
    struct stat stat1;
    struct stat stat2;
    if (fstat(firstFD, &stat1) < 0) return -1;
    if (fstat(secondFD, &stat2) < 0) return -1;
    return ((stat1.st_dev == stat2.st_dev)
            && (stat1.st_ino == stat2.st_ino)
            && (stat1.st_rdev == stat2.st_rdev)
            && (stat1.st_uid) && (stat1.st_uid));
     
}

int main(int argc, char** argv) {
    int fd = open("fifo", O_RDWR);
    int fd2 = open("fifo", O_RDWR);
    for(int i = 0; i < 1000000; i++) {
        if (fileDescriptorsInternalsComparator(fd, fd2) != 1) {
            printf("Error\n");
        }
    }
    return 0;
}



