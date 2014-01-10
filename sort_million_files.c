#include <min_heap.h>

boolean generate_million_files (char *dest_dir) {
    return true;
}

int main(int argc, char *argv[]) {
    char dest_dir[STRUCT_NAME_LENGTH];

    if (argc == 3) {
        if (strcmp("--generate", argv[1]) == 0) {
            if (strlen(argv[2]) < 1) {
                die("usage: sort_million_files --generate <DESTINATION_PATH> %d 123", 10);
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
            die("usage: sort_million_files --generate <DESTINATION_PATH>");
        }

        return 0;
    }
    else if (argc == 2) {
    }

    return 0;
}
