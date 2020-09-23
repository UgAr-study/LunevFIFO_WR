#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

enum { NUMBYTESREAD = 10 };

int main() {
    int fd_from, splice_ret_val;
    char *myfifo = "/tmp/myfifo";

    while((fd_from = open (myfifo, O_RDONLY)) < 0);
    //printf ("open returned %d\n", fd_from);

    while ((splice_ret_val = splice(fd_from, NULL,
                                    STDOUT_FILENO, NULL,
                                    NUMBYTESREAD, SPLICE_F_MOVE)) != 0) {
        if (splice_ret_val < 0) {
            perror("splice()");
            close (fd_from);
            exit (EXIT_FAILURE);
        }
    }

    printf ("\n");
    close (fd_from);
    return 0;
}


