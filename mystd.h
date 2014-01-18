#ifndef MYSTD_H
#define MYSTD_H

#ifndef _GNU_SOURCE
#undef _GNU_SOURCE
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// errno
#include <errno.h>

typedef short int boolean;
typedef short int bool;
#define TRUE 1
#define true 1
#define FALSE 0
#define false 0

// internal names of structs, kind of debugging
#define STRUCT_NAME_LENGTH 100
#define PATH_LENGTH 1024

// used for die(), debug(), etc.
#define MAX_MSG_SIZE 1024

void die_explaining_errno(char *msg, ...);
void die (char *msg, ...);
void debug(char msg[]);
bool readcmd(char *prompt, char cmd[]);

#endif
