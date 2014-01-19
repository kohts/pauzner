#include <mystd.h>
#include <min_heap.h>

// lstat
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// readdir
#include <dirent.h>

boolean merge_million_files (char *source_dir_name, char *dest_file_name)
{
    heap sorting_heap;
    heap_create(&sorting_heap, "min_heap 1");

    FILE *merged_files[100*100*100 + 1];
    int merged_files_last_used = 0;

    struct stat stat_buf;
    int res;

    res = lstat(dest_file_name, &stat_buf);
    if (res != 0) {
        if (errno == ENOENT) {
        }
        else {
            die_explaining_errno("lstat [%s] returned [%d]", dest_file_name, res);
        }
    }
    else {
        die("[%s] exists, not going to overwrite", dest_file_name);
    }

    res = lstat(source_dir_name, &stat_buf);
    if (res != 0) {
        if (errno == ENOENT) {
            die("source directory [%s] doesn't exist, nothing to merge", source_dir_name);
        }
        else {
            die_explaining_errno("lstat [%s] returned [%d]", source_dir_name, res);
        }
    }
    else {
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

        if (strcmp(".", dir_contents->d_name) == 0 ||
            strcmp("..", dir_contents->d_name) == 0) {
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

            // If the end of the directory stream is reached, NULL is returned and errno is  not  changed.
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

            if (strcmp(".", dir_contents_1->d_name) == 0 ||
                strcmp("..", dir_contents_1->d_name) == 0) {
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

            printf("%s (opened files [%d])\n", source_level_2, merged_files_last_used);

            source_level_2_stream = opendir(source_level_2);
            if (source_level_2_stream == NULL) {
                die_explaining_errno("opendir [%s] returned error", source_level_2);
            }

            boolean read_whole_directory_2 = false;

            READ_DIR_2: while (!read_whole_directory_2) {
                struct dirent *dir_contents_2;

                // If the end of the directory stream is reached, NULL is returned and errno is  not  changed.
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

                if (strcmp(".", dir_contents_2->d_name) == 0 ||
                    strcmp("..", dir_contents_2->d_name) == 0) {
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

                // printf("%s\n", source_level_3);

                merged_files_last_used++;
                merged_files[merged_files_last_used] = fopen(source_level_3, "r");
                if (merged_files[merged_files_last_used] == NULL) {
                    die_explaining_errno("can't read file [%s], already opened [%d]", source_level_3, merged_files_last_used);
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

        if (strcmp(".", dir_contents->d_name) == 0 ||
            strcmp("..", dir_contents->d_name) == 0) {
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
