#include <mystd.h>
//#include <min_heap.h>

// lstat
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// readdir
#include <dirent.h>

boolean merge_million_files (char *source_dir, char *dest_file)
{
//    heap sorting_heap;
//    heap_create(&sorting_heap, "min_heap 1");
    
    return true;
}

boolean generate_million_files (char *dest_dir)
{
    struct stat dest_dir_stat_buf;
    int res;

    res = lstat(dest_dir, &dest_dir_stat_buf);
    if (res != 0) {
        if (errno == ENOENT) {
            res = mkdir(dest_dir, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            if (res != 0) {
                die_explaining_errno("mkdir [%s] returned [%d]", dest_dir, res);
            }

            res = lstat(dest_dir, &dest_dir_stat_buf);
            if (res != 0) {
                die_explaining_errno("lstat [%s] returned [%d] for just created directory", dest_dir, res);
            }
        }
        else {
            die_explaining_errno("lstat [%s] returned [%d]", dest_dir, res);
        }
    }

    if (! (S_ISDIR(dest_dir_stat_buf.st_mode)) ) {
        die("[%s] is not a directory, unable to continue", dest_dir);
    }

    DIR *dest_dir_stream;
    dest_dir_stream = opendir(dest_dir);
    if (dest_dir_stream == NULL) {
        die_explaining_errno("opendir [%s] returned error", dest_dir);
    }

    boolean read_whole_directory = false;
    boolean has_files = false;

    READ_DIR: while (!read_whole_directory) {
        struct dirent *dest_dir_contents;

        // If the end of the directory stream is reached, NULL is returned and errno is  not  changed.
        // If an error occurs, NULL is returned and errno is set appropriately.
        int prev_errno = errno;
        dest_dir_contents = readdir(dest_dir_stream);
        
        if (dest_dir_contents == NULL) {
            if (errno != prev_errno) {
                die_explaining_errno("readdir [%s] indicated an error", dest_dir);
            }
            else {
                read_whole_directory = true;
                goto READ_DIR;
            }
        }

        if (strcmp(".", dest_dir_contents->d_name) == 0 ||
            strcmp("..", dest_dir_contents->d_name) == 0) {
            goto READ_DIR;
        }

        has_files = true;
        read_whole_directory = true;
    }

    res = closedir(dest_dir_stream);
    if (res != 0) {
        die_explaining_errno("closedir [%s] returned [%d]", dest_dir, res);
    }

    if (has_files == true) {
        die("directory [%s] is not empty, refusing to generate files", dest_dir);
    }

    char dest_level_1[PATH_LENGTH];
    char dest_level_2[PATH_LENGTH];
    char dest_level_3[PATH_LENGTH];
    char tmp_name[PATH_LENGTH];
    int i, j, k;

    for (i = 0; i <= 99; i++) {
        strcpy(dest_level_1, "");

        if (sprintf(tmp_name, "%02d", i) < 0) {
            die_explaining_errno("can't write to string");
        }
        
        strncat(dest_level_1, dest_dir, (size_t) PATH_LENGTH);
        strncat(dest_level_1, "/", (size_t) PATH_LENGTH);
        strncat(dest_level_1, tmp_name, (size_t) PATH_LENGTH);

        printf("%s\n", dest_level_1);
        
        res = mkdir(dest_level_1, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        if (res != 0) {
            die_explaining_errno("mkdir [%s] returned [%d]", dest_level_1, res);
        }
        
        for(j = 0; j <= 99; j++) {
            strcpy(dest_level_2, dest_level_1);

            if (sprintf(tmp_name, "%02d", j) < 0) {
                die_explaining_errno("can't write to string");
            }

            strncat(dest_level_2, "/", (size_t) PATH_LENGTH);
            strncat(dest_level_2, tmp_name, (size_t) PATH_LENGTH);

            printf("%s\n", dest_level_2);

            res = mkdir(dest_level_2, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            if (res != 0) {
                die_explaining_errno("mkdir [%s] returned [%d]", dest_level_2, res);
            }

            for(k = 0; k <= 99; k++) {
                strcpy(dest_level_3, dest_level_2);

                if (sprintf(tmp_name, "%02d", k) < 0) {
                    die_explaining_errno("can't write to string");
                }

                strncat(dest_level_3, "/", (size_t) PATH_LENGTH);
                strncat(dest_level_3, tmp_name, (size_t) PATH_LENGTH);

//                printf("%s\n", dest_level_3);

                FILE *dest_file;
                dest_file = fopen(dest_level_3, "w");
                if (dest_file == NULL) {
                    die_explaining_errno("can't write to file [%s]", dest_level_3);
                }

                fprintf(dest_file, "%d\n", 10000 * i + 100 * j + k);

                if (fclose(dest_file) != 0) {
                    die_explaining_errno("can't close file [%s]", dest_level_3);
                }
            }
        }
    }

    return true;
}

void usage()
{
    die("%s\n%s\n%s\n\n",
        "  usage: ",
        "    merge_million_files --generate <DESTINATION_PATH> <NUMBER_OF_FILES>",
        "    merge_million_files --merge <SOURCE_PATH> --dest <FILE>"
        );
}

int main(int argc, char *argv[])
{
    char dest_dir[PATH_LENGTH];

    if (argc == 5) {
        if (strcmp("--merge", argv[1]) == 0 && strcmp("--dest", argv[3]) == 0) {
            printf("here\n");
        }
        else {
            usage();
        }
    }
    else if (argc == 4) {
        if (strcmp("--generate", argv[1]) == 0) {
            if (strlen(argv[2]) < 1) {
                die("usage example: merge_million_files --generate <DESTINATION_PATH> 10");
            }
            else if (strlen(argv[2]) > PATH_LENGTH) {
                die("<DESTINATION_DIR> must be no longer than [%d]", PATH_LENGTH);
            }

            strcpy(dest_dir, argv[2]);
            
            if (generate_million_files(dest_dir) == true) {
                printf("finished successfully.\n");
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
