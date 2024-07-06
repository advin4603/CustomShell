#include "headers.h"

bool change_directory(char *directory_path) {
    if (*directory_path == '-' && !*(directory_path + 1)) {
        if (global_config.previous_working_directory == NULL) {
            print_error("Couldn't Change Directory: No Previous Directory\n");
            return false;
        }
        if (chdir(global_config.previous_working_directory) == -1) {
            print_errno_error("Couldn't Change Directory: %s\n");
            return false;
        }
        return true;
    }
    if (chdir(directory_path) == -1) {
        print_errno_error("Couldn't Change Directory: %s\n");
        return false;
    }
    return true;
}

void warp(size_t argc, char **argv) {
    if (argc == 1) {
        change_directory(global_config.home_directory_path);
        char *current_working_directory = getcwd(NULL, 0);
        printf("%s\n", current_working_directory);
        free(current_working_directory);
        return;
    }
    for (size_t i = 1; i < argc; i++) {
        char *previous_working_directory = getcwd(NULL, 0);
        bool warp_successful = change_directory(argv[i]);
        if (!warp_successful) {
            free(previous_working_directory);
            break;
        }

        if (global_config.previous_working_directory != NULL)
            free(global_config.previous_working_directory);
        global_config.previous_working_directory = previous_working_directory;
        char *current_working_directory = getcwd(NULL, 0);
        printf("%s\n", current_working_directory);
        free(current_working_directory);
    }
}
