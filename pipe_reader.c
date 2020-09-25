#include "fifo_header.h"

enum { NUMBYTESREAD = 10 };

int main() {
    int fd_from, fd_send_pid, splice_ret_val;
    char *send_pid_fifo = "/tmp/send_pid_fifo", *special_fifo;

    pid_t curr_pid = getpid();
    special_fifo = GenerateFifoname(curr_pid);

    printf ("\nthe unique name is %s\n\n", special_fifo);

    CreateFifo(special_fifo, 0666);
    fd_from = OpenFile(special_fifo, O_RDONLY | O_NONBLOCK);

    CreateFifo(send_pid_fifo, 0666);
    fd_send_pid = OpenFile (send_pid_fifo, O_WRONLY);

    if (write (fd_send_pid, &curr_pid, sizeof(pid_t)) <= 0) {
        printf ("error of write\n");
        exit (EXIT_FAILURE);
    }

    int check_sp_fifo = CheckSelect(fd_from, -1, -1);
    if  (check_sp_fifo < 0) {
        printf ("the writer is not ready for too long\n");
        exit (EXIT_FAILURE);
    }
    fcntl(fd_from, F_SETFL, O_RDONLY);
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
    close (fd_send_pid);
    close (fd_from);
    return 0;
}