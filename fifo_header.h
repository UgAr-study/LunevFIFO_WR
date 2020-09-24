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

char * generate_fifoname (pid_t pid);