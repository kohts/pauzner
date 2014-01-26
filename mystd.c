#include <mystd.h>

void die_explaining_errno(char *msg, ...) {
    int orig_errno = errno;

    char orig_errno_explained[PATH_LENGTH];
    char orig_msg[PATH_LENGTH];

    va_list ap;
    va_start(ap, msg);

    vsnprintf(orig_msg, (size_t) PATH_LENGTH, msg, ap);

    if (strerror_r(errno, orig_errno_explained, (size_t) PATH_LENGTH) == 0) {
        fprintf(stderr, "%s, errno [%d]: %s\n", orig_msg, orig_errno, orig_errno_explained);
    }
    else {
        fprintf(stderr, "%s, errno [%d], additionally strerror_r() failed with errno [%d]\n", orig_msg, orig_errno, errno);
    }

    va_end(ap);
    exit(1);
}

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
