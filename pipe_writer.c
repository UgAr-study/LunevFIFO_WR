#define _GNU_SOURCE
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/shm.h>
#include <errno.h>

enum { NUMBYTESREAD = 10 };

int is_fifo (const char* filename);

int main(int argc, char* argv[]) {

    int fd_in, fd_from;
    ssize_t splice_ret_val;
    char *myfifo = "/tmp/myfifo";

    if (argc == 1) {
        printf ("not all arguments\n");
        exit (EXIT_FAILURE);
    }

    if (argc > 2) {
        if (strcmp(argv[2], "unlink") == 0) {
            unlink(myfifo);
        }
    }

    if (is_fifo(myfifo) == 0) {
        printf ("\nfifo is already busy\n");
        exit (EXIT_FAILURE);
    }

    (void) umask(0);

    mkfifo (myfifo, 0666);

    if ((fd_in = open (myfifo, O_WRONLY)) < 0) {
        perror("open()");
        unlink (myfifo);
        exit (EXIT_FAILURE);
    }

    if ( (fd_from = open (argv[1], O_RDONLY)) < 0){
        perror("open()");
        unlink (myfifo);
        exit (EXIT_FAILURE);
    }

    while ((splice_ret_val = splice(fd_from, NULL,
                                   fd_in, NULL,
                                   NUMBYTESREAD, SPLICE_F_MOVE)) != 0) {
        if (splice_ret_val < 0) {
            perror("splice()");
            close (fd_in);
            close (fd_from);
            unlink (myfifo);
            exit (EXIT_FAILURE);
        }

        printf ("spliced: %zu\n", splice_ret_val);
    }
    close (fd_from);
    close (fd_in);
    unlink(myfifo);
    return 0;
}

int is_fifo (const char* filename) {
    struct stat st;
    stat (filename, &st);
    if (S_ISFIFO(st.st_mode) != 0) {
        printf("%s is FIFO\n", filename);
        return 0;
    }
    else
        printf ("%s is not FIFO\n", filename);
    return -1;
}
