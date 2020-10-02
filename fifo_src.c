#include "fifo_header.h"

char * GenerateFifoname (pid_t pid) {
    char* res, filepath[11] = "/tmp/nfifo", strpid[10] = {0};
    res = (char*) calloc (21, sizeof(char));
    sprintf (strpid, "%d", pid);
    strcpy (res, filepath);
    strcat (res, strpid);
    return res;
}

int CheckPid (char* fifoname, pid_t pid) {
    int fd_check;
    pid_t key;

    fd_check = OpenFile(fifoname, O_RDONLY | O_NONBLOCK);

    if (CheckSelect(fd_check, -1, -1) < 0) {
        close(fd_check);
        return -1;
    }

    if (read (fd_check, &key, sizeof(pid_t)) < 0) {
        printf ("smth goes wrong\n");
        close(fd_check);
        return -1;
    }

    if (key != pid) {
        printf ("i don't have a pair\n");
        close(fd_check);
        return -1;
    }

    close (fd_check);
    return 0;
}

int OpenFile (const char* filename, int flags) {
    int fd;
    if ((fd = open (filename, flags)) < 0) {
        printf ("i can't\n");
        perror("open()");
        exit (EXIT_FAILURE);
    }
    return fd;
}

void CreateFifo (const char* fifoname, mode_t mode) {
    (void) umask(0);
    if (mkfifo (fifoname, mode) < 0) {
        if (errno != EEXIST) {
            perror ("mkfifo()");
            exit (EXIT_FAILURE);
        }
    }
}

int CheckSelect (int fdread, int fdwrite, int fdexe) {
    fd_set rset, wset, eset;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    int max = fdread;
    if (fdwrite > max)
        max = fdwrite;
    if (fdexe > max)
        max = fdexe;

    FD_ZERO (&rset);
    FD_ZERO (&wset);
    FD_ZERO (&eset);

    if (fdread != -1)
        FD_SET(fdread, &rset);
    if (fdwrite != -1)
        FD_SET(fdwrite, &wset);
    if (fdexe != -1)
        FD_SET(fdexe, &eset);

    errno = 0;
    int sel = select (max + 1, &rset, &wset, &eset, &tv);

    if (sel <= 0) {
        perror ("select()");
        return -1;
    }

    return sel;
}