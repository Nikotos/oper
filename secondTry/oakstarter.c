#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc > 1) {
        execv(argv[1], argv + 1);
    }
    return 0;
}

