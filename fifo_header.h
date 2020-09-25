#pragma once

#define _GNU_SOURCE
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/shm.h>
#include <errno.h>

char * GenerateFifoname (pid_t pid);
int CheckPid (char* fifoname, pid_t pid);
int OpenFile (const char* filename, int flags);
void CreateFifo (const char* fifoname, mode_t mode);
int CheckSelect (int fdread, int fdwrite, int fdexe);