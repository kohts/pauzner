// O_NOATIME, O_DIRECT
#define _GNU_SOURCE

#include <mystd.h>
#include <min_heap.h>

// lstat
#include <sys/types.h>
#include <sys/stat.h>

// read
#include <unistd.h>

// open
#include <fcntl.h>

// readdir
#include <dirent.h>

// isspace
#include <ctype.h>

// INT_MIN, INT_MAX
#include <limits.h>

// reads the open fd byte by byte till the next empty space or \n,
// skips the space before the number
//
// returns true if the number was found and written to *number_storage,
// false otherwise
//
// if fd reaches its end during read operation,
// closes it and updates fd storage with -1
//
boolean read_number (int *fd, int *number_storage)
{
    char str_number[PATH_LENGTH];
    int free_char = 0;

    bool finish_reading = false;

    READ_NUMBER: while (!finish_reading) {
        char current_byte;

        int read_bytes = read(*fd, &current_byte, 1);

        if (read_bytes == -1) {
            die_explaining_errno("read on fd [%d] returned [-1]", *fd);
        }
        if (read_bytes == 0) {
            *fd = -1;
            finish_reading = true;
            goto READ_NUMBER;
        }
        if (isspace(current_byte)) {
            if (free_char != 0) {
                finish_reading = true;
            }
            
            goto READ_NUMBER;
        }

        if (free_char > PATH_LENGTH) {
            die("no empty space to write next byte of the number, fd [%d], read [%c]", *fd, current_byte);
        }

        str_number[free_char++] = current_byte;
    }

    if (free_char == 0) {
        return false;
    }

    str_number[free_char] = '\0';

    errno = 0;
    long int tmp_int = strtol(str_number, (char **) NULL, 10);
    if (errno != 0) {
        die_explaining_errno("got not a number [%s] in fd [%d]", str_number, *fd);
    }
    if (tmp_int < INT_MIN || tmp_int > INT_MAX) {
        die("got too big number for sorting heap [%d]", tmp_int);
    }

    *number_storage = tmp_int;

    return true;
}

boolean merge_million_files (char *source_dir_name, char *dest_file_name)
{
    int merged_files_fd[1 + 100*100*100];

    // segfaults if both arrays are defined (?)
//    FILE *merged_files[1 + 100*100*100];

    int merged_files_last_used = 0;

    struct stat stat_buf;
    int res;

    res = lstat(dest_file_name, &stat_buf);
    if (res != 0) {
        if (errno == ENOENT) {

        } else {
            die_explaining_errno("lstat [%s] returned [%d]", dest_file_name, res);
        }
    } else {
        die("[%s] exists, not going to overwrite", dest_file_name);
    }

    res = lstat(source_dir_name, &stat_buf);
    if (res != 0) {
        if (errno == ENOENT) {
            die("source directory [%s] doesn't exist, nothing to merge", source_dir_name);
        } else {
            die_explaining_errno("lstat [%s] returned [%d]", source_dir_name, res);
        }
    } else {
        if (! (S_ISDIR(stat_buf.st_mode)) ) {
            die("[%s] is not a directory, unable to continue", source_dir_name);
        }
    }

    FILE *dest_file;
    dest_file = fopen(dest_file_name, "w");
    if (dest_file == NULL) {
        die_explaining_errno("can't write to file [%s]", dest_file_name);
    }

    DIR *source_dir_stream;
    source_dir_stream = opendir(source_dir_name);
    if (source_dir_stream == NULL) {
        die_explaining_errno("opendir [%s] returned error", source_dir_name);
    }

    boolean read_whole_directory = false;

    char source_level_1[PATH_LENGTH];
    char source_level_2[PATH_LENGTH];
    char source_level_3[PATH_LENGTH];
    DIR *source_level_1_stream;
    DIR *source_level_2_stream;

    printf("reading files from [%s]\n", source_dir_name);

    READ_DIR: while (!read_whole_directory) {
        struct dirent *dir_contents;

        // If the end of the directory stream is reached, NULL is returned and errno is  not  changed.
        // If an error occurs, NULL is returned and errno is set appropriately.
        int prev_errno = errno;
        dir_contents = readdir(source_dir_stream);
        
        if (dir_contents == NULL) {
            if (errno != prev_errno) {
                die_explaining_errno("readdir [%s] indicated an error", source_dir_name);
            }
            else {
                read_whole_directory = true;
                goto READ_DIR;
            }
        }

        if (strcmp(".", dir_contents->d_name) == 0 || strcmp("..", dir_contents->d_name) == 0) {
            goto READ_DIR;
        }

        strcpy(source_level_1, "");
        strncat(source_level_1, source_dir_name, (size_t) PATH_LENGTH);
        strncat(source_level_1, "/", (size_t) PATH_LENGTH);
        strncat(source_level_1, dir_contents->d_name, (size_t) PATH_LENGTH);

        res = lstat(source_level_1, &stat_buf);
        if (res != 0) {
            die_explaining_errno("lstat [%s] returned [%d]", source_level_1, res);
        }
        else {
            if (! (S_ISDIR(stat_buf.st_mode)) ) {
                die("[%s] is not a directory, unexpected source directory format, unable to continue.", source_level_1, res);
            }
        }

        printf("%s\n", source_level_1);

        source_level_1_stream = opendir(source_level_1);
        if (source_level_1_stream == NULL) {
            die_explaining_errno("opendir [%s] returned error", source_level_1);
        }

        boolean read_whole_directory_1 = false;

        READ_DIR_1: while (!read_whole_directory_1) {
            struct dirent *dir_contents_1;

            // If the end of the directory stream is reached, NULL is returned and errno is not changed.
            // If an error occurs, NULL is returned and errno is set appropriately.
            int prev_errno_1 = errno;
            dir_contents_1 = readdir(source_level_1_stream);
            
            if (dir_contents_1 == NULL) {
                if (errno != prev_errno_1) {
                    die_explaining_errno("readdir [%s] indicated an error", source_level_1);
                }
                else {
                    read_whole_directory_1 = true;
                    goto READ_DIR_1;
                }
            }

            if (strcmp(".", dir_contents_1->d_name) == 0 || strcmp("..", dir_contents_1->d_name) == 0) {
                goto READ_DIR_1;
            }
        
            strcpy(source_level_2, "");
            strncat(source_level_2, source_level_1, (size_t) PATH_LENGTH);
            strncat(source_level_2, "/", (size_t) PATH_LENGTH);
            strncat(source_level_2, dir_contents_1->d_name, (size_t) PATH_LENGTH);

            res = lstat(source_level_2, &stat_buf);
            if (res != 0) {
                die_explaining_errno("lstat [%s] returned [%d]", source_level_2, res);
            }
            else {
                if (! (S_ISDIR(stat_buf.st_mode)) ) {
                    die("[%s] is not a directory, unexpected source directory format, unable to continue.", source_level_2, res);
                }
            }

            //printf("%s (opened files [%d])\n", source_level_2, merged_files_last_used);

            source_level_2_stream = opendir(source_level_2);
            if (source_level_2_stream == NULL) {
                die_explaining_errno("opendir [%s] returned error", source_level_2);
            }

            boolean read_whole_directory_2 = false;

            READ_DIR_2: while (!read_whole_directory_2) {
                struct dirent *dir_contents_2;

                // If the end of the directory stream is reached, NULL is returned and errno is not changed.
                // If an error occurs, NULL is returned and errno is set appropriately.
                int prev_errno_2 = errno;
                dir_contents_2 = readdir(source_level_2_stream);
                
                if (dir_contents_2 == NULL) {
                    if (errno != prev_errno_2) {
                        die_explaining_errno("readdir [%s] indicated an error", source_level_2);
                    }
                    else {
                        read_whole_directory_2 = true;
                        goto READ_DIR_2;
                    }
                }

                if (strcmp(".", dir_contents_2->d_name) == 0 || strcmp("..", dir_contents_2->d_name) == 0) {
                    goto READ_DIR_2;
                }

                strcpy(source_level_3, "");
                strncat(source_level_3, source_level_2, (size_t) PATH_LENGTH);
                strncat(source_level_3, "/", (size_t) PATH_LENGTH);
                strncat(source_level_3, dir_contents_2->d_name, (size_t) PATH_LENGTH);

                res = lstat(source_level_3, &stat_buf);
                if (res != 0) {
                    die_explaining_errno("lstat [%s] returned [%d]", source_level_3, res);
                }
                else {
                    if (! (S_ISREG(stat_buf.st_mode)) ) {
                        die("[%s] is not a file, unexpected source directory format, unable to continue.", source_level_3, res);
                    }
                }

//                printf("%s\n", source_level_3);

                merged_files_last_used++;

                merged_files_fd[merged_files_last_used] = open(source_level_3, O_RDONLY);
                if (merged_files_fd[merged_files_last_used] == -1) {
                    die_explaining_errno("can't read file [%s], already opened [%d]", source_level_3, merged_files_last_used);
                }

/*                merged_files[merged_files_last_used] = fopen(source_level_3, "r");
                if (merged_files[merged_files_last_used] == NULL) {
                    die_explaining_errno("can't read file [%s], already opened [%d]", source_level_3, merged_files_last_used);
                }
*/
                // do not read more than we can process (sorry)
                if (merged_files_last_used >= 200000) {
                    read_whole_directory_2 = true;
                    read_whole_directory_1 = true;
                    read_whole_directory = true;
                }
            }

            res = closedir(source_level_2_stream);
            if (res != 0) {
                die_explaining_errno("closedir [%s] returned [%d]", source_level_2, res);
            }

        }
        
        res = closedir(source_level_1_stream);
        if (res != 0) {
            die_explaining_errno("closedir [%s] returned [%d]", source_level_1, res);
        }
        
//    fprintf(dest_file, "%d\n", 10000 * i + 100 * j + k);
    }

    res = closedir(source_dir_stream);
    if (res != 0) {
        die_explaining_errno("closedir [%s] returned [%d]", source_dir_name, res);
    }

    printf("opened [%d] files, populating heap\n", merged_files_last_used);

    // do the heap
    heap sorting_heap;
    heap_create(&sorting_heap, "min_heap 1");

    int i;
    int tmp;

    FOR_EACH_OPEN_FILE: for (i = 1; i <= merged_files_last_used; i++) {
        // this file has been already read
        if (merged_files_fd[i] == -1) {
            next FOR_EACH_OPEN_FILE;
        }
        
        // nothing to read from this file
        if (read_number(&merged_files_fd[i], &tmp) == false) {
            next FOR_EACH_OPEN_FILE;
        }

        // got new number, feed to the heap
//        printf("read [%d] from %d fd [%d]\n", tmp, i, merged_files_fd[i]);
        heap_insert(&sorting_heap, tmp, (void *) &merged_files_fd[i]);
    }

//    heap_dump_structured(&sorting_heap);
//    heap_dump_storage(&sorting_heap);

    printf("heap populated, merging\n");

    while (heap_is_empty(&sorting_heap) == false) {
        int min_key;
        void *min_fd = NULL;
        
        if (heap_extract_min(&sorting_heap, &min_key, &min_fd) != NULL) {
            // append to the sorted output file
            fprintf(dest_file, "%d\n", min_key);

            if (min_fd == NULL) {
                die("heap data corruption: got NULL min_fd for key [%d]", min_key);
            }
            
//            printf("key [%d], fd [%d]\n", min_key, *((int *) min_fd));

            if (read_number((int *)min_fd, &tmp) == true) {
                heap_insert(&sorting_heap, tmp, min_fd);
            }
        }
    }

    if (fclose(dest_file) != 0) {
        die_explaining_errno("can't close file [%s]", dest_file_name);
    }
    
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
    else {
        if (! (S_ISDIR(dest_dir_stat_buf.st_mode)) ) {
            die("[%s] is not a directory, unable to continue", dest_dir);
        }
    }

    DIR *dest_dir_stream;
    dest_dir_stream = opendir(dest_dir);
    if (dest_dir_stream == NULL) {
        die_explaining_errno("opendir [%s] returned error", dest_dir);
    }

    boolean read_whole_directory = false;
    boolean has_files = false;

    READ_DIR: while (!read_whole_directory) {
        struct dirent *dir_contents;

        // If the end of the directory stream is reached, NULL is returned and errno is  not  changed.
        // If an error occurs, NULL is returned and errno is set appropriately.
        int prev_errno = errno;
        dir_contents = readdir(dest_dir_stream);
        
        if (dir_contents == NULL) {
            if (errno != prev_errno) {
                die_explaining_errno("readdir [%s] indicated an error", dest_dir);
            }
            else {
                read_whole_directory = true;
                goto READ_DIR;
            }
        }

        if (strcmp(".", dir_contents->d_name) == 0 || strcmp("..", dir_contents->d_name) == 0) {
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
        "    merge_million_files --merge <SOURCE_PATH> --dest <MERGED_FILE>"
        );
}

int main(int argc, char *argv[])
{
    char dest_dir[PATH_LENGTH];
    char dest_file[PATH_LENGTH];

    if (argc == 5) {
        if (strcmp("--merge", argv[1]) == 0 && strcmp("--dest", argv[3]) == 0) {
            if (strlen(argv[2]) < 1) {
                usage();
            }
            else if (strlen(argv[2]) > PATH_LENGTH) {
                die("<SOURCE_PATH> must be no longer than [%d]", PATH_LENGTH);
            }

            if (strlen(argv[4]) < 1) {
                usage();
            }
            else if (strlen(argv[4]) > PATH_LENGTH) {
                die("<MERGED_FILE> must be no longer than [%d]", PATH_LENGTH);
            }

            strcpy(dest_dir, argv[2]);
            strcpy(dest_file, argv[4]);

            if (merge_million_files(dest_dir, dest_file) == true) {
                printf("finished successfully.\n");
            }
            else {
                die("error merging files");
            }
        }
        else {
            usage();
        }
    }
    else if (argc == 3) {
        if (strcmp("--generate", argv[1]) == 0) {
            if (strlen(argv[2]) < 1) {
                die("usage example: merge_million_files --generate <DESTINATION_PATH>");
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
