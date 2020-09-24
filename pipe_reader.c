#include "fifo_header.h"

enum { NUMBYTESREAD = 10 };

int main() {
    int fd_from, fd_receive_pid, fd_send_sig, splice_ret_val;
    char *send_pid_fifo = "/tmp/send_pid_fifo", *special_fifo, unique_key[10] = {0};

    while((fd_receive_pid = open (send_pid_fifo, O_RDONLY)) < 0);

    int read_ret_val;
    if ((read_ret_val = read (fd_receive_pid, unique_key, 10)) <= 0) {
        printf ("reader don't has a pair\n");
        close (fd_receive_pid);
        exit (EXIT_FAILURE);
    }

    close (fd_receive_pid);
    int key = (int) strtol (unique_key, NULL, 0);
    special_fifo = generate_fifoname(key);

    printf ("\n\n%d bytes are read;\nkey = %d;\nunique fifo name is: [%s]\n\n",
            read_ret_val, key, special_fifo);

    if ((fd_send_sig = open (special_fifo, O_WRONLY)) < 0) {
        printf ("reader send signal error\n");
        exit (EXIT_FAILURE);
    }

    if (dprintf(fd_send_sig, "%d ", key) <= 0) {
        printf ("reader send signal error\n");
        close(fd_send_sig);
        exit (EXIT_FAILURE);
    }

    close (fd_send_sig);

    while ((fd_from = open (special_fifo, O_RDONLY)) < 0);

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
    close (fd_receive_pid);
    close (fd_from);
    return 0;
}