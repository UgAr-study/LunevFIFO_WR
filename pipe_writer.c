#include "fifo_header.h"

enum { NUMBYTESREAD = 10 };

int main(int argc, char* argv[]) {
    int fd_receive_pid, fd_in, fd_from;
    ssize_t splice_ret_val;
    char *send_pid_fifo = "/tmp/send_pid_fifo", *text_fifo;
    pid_t key;

    if (argc == 1) {
        printf ("not all arguments\n");
        exit (EXIT_FAILURE);
    }

    fd_from = OpenFile(argv[1], O_RDONLY);

    CreateFifo(send_pid_fifo, 0666);

    fd_receive_pid = OpenFile(send_pid_fifo, O_RDONLY);
    sleep(5);
    int read_ret_val;
    if ((read_ret_val = read (fd_receive_pid, &key, sizeof(pid_t))) <= 0) {
        printf ("writer don't has a pair\n");
        close (fd_receive_pid);
        exit (EXIT_FAILURE);
    }

    //sleep (3);

    text_fifo = GenerateFifoname(key);
    printf ("\n\n%d bytes are read;\nkey = %d;\nunique fifo name is: [%s]\n\n",
            read_ret_val, key, text_fifo);

    CreateFifo(text_fifo, 0666);

    fd_in = OpenFile(text_fifo, O_WRONLY | O_NONBLOCK);
    fcntl(fd_in, F_SETFL, O_WRONLY);

    while ((splice_ret_val = splice(fd_from, NULL,
                                   fd_in, NULL,
                                   NUMBYTESREAD, SPLICE_F_MOVE)) != 0) {
        if (splice_ret_val < 0) {
            printf ("in writing:\n");
            perror("splice()");
            close (fd_in);
            close (fd_from);
            close (fd_receive_pid);
            exit (EXIT_FAILURE);
        }

        printf ("spliced: %zu\n", splice_ret_val);
    }

    close (fd_from);
    close (fd_receive_pid);
    unlink (send_pid_fifo);
    return 0;
}