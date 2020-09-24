#include "fifo_header.h"

char * generate_fifoname (pid_t pid) {
    char* res, filepath[11] = "/tmp/nfifo", strpid[10] = {0};
    res = (char*) calloc (21, sizeof(char));
    sprintf (strpid, "%d", pid);
    strcpy (res, filepath);
    strcat (res, strpid);
    return res;
}
