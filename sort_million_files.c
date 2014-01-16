#include <min_heap.h>

// lstat
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// errno
#include <errno.h>

boolean generate_million_files (char *dest_dir) {
    struct stat stat_buf;

    int res = lstat(dest_dir, &stat_buf);
    if (res != 0) {
        int orig_errno = errno;
        char error_message[STRUCT_NAME_LENGTH];

        if (strerror_r(errno, error_message, (size_t) STRUCT_NAME_LENGTH) == 0) {
            die("lstat [%s] returned [%d]: %s", dest_dir, res, error_message);
        }
        else {
            die("lstat [%s] returned [%d]: errno %d, additionally strerror_r failed with errno %d", dest_dir, res, orig_errno, errno);
        }
    }

    if (S_ISDIR(stat_buf.st_mode)) {
        printf("%s is dir\n", dest_dir);
    }
    
    return true;
}

void usage()
{
    die("%s\n%s\n%s\n\n",
        "  usage: ",
        "    sort_million_files --generate <DESTINATION_PATH> <NUMBER_OF_FILES>",
        "    sort_million_files --sort <SOURCE_PATH> --dest <FILE>"
        );
}

int main(int argc, char *argv[])
{
    char dest_dir[STRUCT_NAME_LENGTH];

    if (argc == 5) {
        if (strcmp("--sort", argv[1]) == 0 && strcmp("--dest", argv[3]) == 0) {
            printf("here\n");
        }
        else {
            usage();
        }
    }
    else if (argc == 4) {
        if (strcmp("--generate", argv[1]) == 0) {
            if (strlen(argv[2]) < 1) {
                die("usage example: sort_million_files --generate <DESTINATION_PATH> 10");
            }
            else if (strlen(argv[2]) > STRUCT_NAME_LENGTH) {
                die("<DESTINATION_DIR> must be no longer than %d", STRUCT_NAME_LENGTH);
            }

            strcpy(dest_dir, argv[2]);
            
            if (generate_million_files(dest_dir) == true) {
                printf("done\n");
            }
            else {
                die("error generating files");
            }
        }
        else {
            usage();
        }

        return 0;
    }
    else {
        usage();
    }

    return 0;
}
