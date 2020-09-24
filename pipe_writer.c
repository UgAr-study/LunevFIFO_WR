#include "fifo_header.h"

enum { NUMBYTESREAD = 10 };

int check_pid (char* fifoname, pid_t pid);

int main(int argc, char* argv[]) {
    pid_t curr_pid;
    int fd_send_pid, fd_in, fd_from, fd_check;
    ssize_t splice_ret_val;
    char *send_pid_fifo = "/tmp/send_pid_fifo", *text_fifo;

    if (argc == 1) {
        printf ("not all arguments\n");
        exit (EXIT_FAILURE);
    }

    (void) umask(0);

    mkfifo (send_pid_fifo, 0666);

    if ((fd_send_pid = open (send_pid_fifo, O_WRONLY)) < 0) {
        printf ("i can't\n");
        perror("open()");
        unlink (send_pid_fifo);
        exit (EXIT_FAILURE);
    }

    curr_pid = getpid();
    if (dprintf (fd_send_pid, "%d ", curr_pid) <= 0) {
        printf ("error of dprintf\n");
        exit (EXIT_FAILURE);
    }


    if ((fd_from = open (argv[1], O_RDONLY)) < 0) {
        printf ("i can't\n");
        perror("open()");
        exit (EXIT_FAILURE);
    }

    text_fifo = generate_fifoname (curr_pid);
    mkfifo (text_fifo, 0666);

    if ((fd_check = open (text_fifo, O_RDONLY | O_NONBLOCK)) < 0) {
        printf ("can't open my special fifo for reading\n");
        exit (EXIT_FAILURE);
    }

    if (check_pid(text_fifo, curr_pid) == -1) {
        printf ("wrong pid\n");
        exit(EXIT_FAILURE);
    }
    fd_set rset;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    FD_ZERO (&rset);
    FD_SET(fd_check, &rset);

    int sel = select (fd_check + 1, &rset, NULL, NULL, &tv);

    if (sel <= 0) {
        printf ("smth goes wrong, may be i don't have a pair\n");
        close(fd_check);
        close(fd_from);
        exit (EXIT_FAILURE);
    }

    char check_pid[10] = {0};
    if (read (fd_check, check_pid, 10) < 0) {
        printf ("smth goes wrong\n");
        close(fd_check);
        close(fd_from);
        exit (EXIT_FAILURE);
    }

    if (strtol(check_pid, NULL, 0) != curr_pid) {
        printf ("i don't have a pair\n");
        close(fd_check);
        close(fd_from);
        exit (EXIT_FAILURE);
    }
    close (fd_check);

    if ((fd_in = open (text_fifo, O_WRONLY)) < 0) {
        printf ("can't open my special fifo\n");
        exit(EXIT_FAILURE);
    }

    while ((splice_ret_val = splice(fd_from, NULL,
                                   fd_in, NULL,
                                   NUMBYTESREAD, SPLICE_F_MOVE)) != 0) {
        if (splice_ret_val < 0) {
            printf ("in writing:\n");
            perror("splice()");
            close (fd_in);
            close (fd_from);
            close (fd_send_pid);
            exit (EXIT_FAILURE);
        }

        printf ("spliced: %zu\n", splice_ret_val);
    }
    close (fd_from);
    close (fd_in);
    close (fd_send_pid);
    unlink (send_pid_fifo);
    return 0;
}

int check_pid (char* fifoname, pid_t pid) {
    int fd_check;
    fd_set rset;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    if ((fd_check = open (fifoname, O_RDONLY | O_NONBLOCK)) < 0) {
        printf ("can't open my special fifo for reading\n");
        exit (EXIT_FAILURE);
    }

    FD_ZERO (&rset);
    FD_SET(fd_check, &rset);

    int sel = select (fd_check + 1, &rset, NULL, NULL, &tv);

    if (sel <= 0) {
        printf ("smth goes wrong, may be i don't have a pair\n");
        close(fd_check);
        return -1;
    }

    char check_pid[10] = {0};
    if (read (fd_check, check_pid, 10) < 0) {
        printf ("smth goes wrong\n");
        close(fd_check);
        return -1;
    }

    if (strtol(check_pid, NULL, 0) != pid) {
        printf ("i don't have a pair\n");
        close(fd_check);
        return -1;
    }
    close (fd_check);
    return 0;
}