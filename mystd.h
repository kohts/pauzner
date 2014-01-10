#ifndef MYSTD_H
#define MYSTD_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef short int boolean;
typedef short int bool;
#define TRUE 1
#define true 1
#define FALSE 0
#define false 0

// global debugging mode
boolean DEBUG = FALSE;

// internal names of structs, kind of debugging
#define STRUCT_NAME_LENGTH 100

void die (char *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    vfprintf(stderr, msg, ap);

    size_t msg_len = strlen(msg);
    if (msg[msg_len - 1] != '\n') {
        fprintf(stderr, "\n");
    }

    va_end(ap);
    exit(1);
}

void debug(char msg[]) {
    fprintf(stderr, "debug: %s\n", msg);
}

// used for die(), debug(), etc.
#define MAX_MSG_SIZE 1024

// interactive "shell" command prompt used to test structures
bool readcmd(char *prompt, char cmd[]) {
    int i,c;
    int done=0;
    
    printf("%s", prompt);
    for (i=0; done == 0; i++) {
        if (i < MAX_MSG_SIZE - 1) {
            if ( (c = getchar()) != '\n') {
                if (c == EOF) {
                    cmd[i] = 'q';
                    i++;
                }
                else {
                    cmd[i] = c;
                    continue;
                }
            }
        }

        // exit
        done = 1;
    }
    cmd[i - 1] = 0;

    return TRUE;
}

#endif
