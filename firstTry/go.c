#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>



int main(int argc, char** argv) {
    if (argc > 1) {
        char* endptr = NULL;
        long int number = strtol(argv[1], &endptr, 0);
        
        if (number == 0) {
            printf("Inappropriate Symbol!\n");
        }
        else if (*endptr != "/n") {
            printf("Inappropriate Symbol in the end - [%c]\n", *endptr);
        }
        else if (errno == ERANGE) {
            printf("Number out of range!\n");
        }
        else if (number > 0) {
            printf("I am counting for the number %ld\n", number);
            for(long int i = 1; i <= number; i++) {
                printf("%d\n", i);
            }
        }
        else if (number < 0) {
            printf("I am counting down for the number %ld\n", number);
            for(long int i = -1; i >= number; i--) {
                printf("%d\n", i);
            }
        }
        
    }
    
    return 0;
}
