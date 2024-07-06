#include "headers.h"

void search(char *target, char *search_directory, bool only_directories_flag, bool only_files_flag, bool *found_one,
            char **last_path, char *base_directory) {
    DIR *directory = opendir(search_directory);
    if (directory == NULL) {
        print_error("Couldn't open ");
        print_error(search_directory);
        print_errno_error(" : %s\n");
        return;
    }

    struct dirent *directory_entry;
    while ((directory_entry = readdir(directory)) != NULL) {
        if (directory_entry->d_name[0] == '.')
            continue; // no hidden files
        bool is_directory = directory_entry->d_type == DT_DIR;
        if (!is_directory && only_directories_flag)
            continue;

        if (is_directory && only_files_flag) {
            char *entry_path = join_paths(search_directory, directory_entry->d_name);
            search(target, entry_path, only_directories_flag, only_files_flag, found_one, last_path, base_directory);
            free(entry_path);
            continue;
        }


        bool is_target = strcmp(directory_entry->d_name, target) == 0;
        if (is_target || is_directory) {
            char *entry_path = join_paths(search_directory, directory_entry->d_name);
            if (is_target) {
                size_t base_directory_length = strlen(base_directory);
                if (*(entry_path + base_directory_length) == '/')
                    base_directory_length++;
                printf(is_directory ? ANSI_COLOR_BLUE"./%s\n"ANSI_COLOR_RESET : ANSI_COLOR_GREEN"./%s\n"ANSI_COLOR_RESET,
                       entry_path + base_directory_length);
                if (*last_path == NULL) {
                    *found_one = true;
                    *last_path = string_duplicate(entry_path);
                } else {
                    *found_one = false;
                }

            }

            if (is_directory)
                search(target, entry_path, only_directories_flag, only_files_flag, found_one, last_path,
                       base_directory);

            free(entry_path);
        }


    }

    closedir(directory);

}

void seek(size_t argc, char **argv) {
    char *target = NULL;
    char *search_directory = ".";
    bool only_directories_flag = false, only_files_flag = false, output_flag = false, custom_search_directory = false;
    for (size_t i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            char *flag_pointer = argv[i];
            while (*(++flag_pointer)) {
                switch (*flag_pointer) {
                    case 'd':
                        only_directories_flag = true;
                        if (only_files_flag) {
                            print_error("Error: seek: Invalid configuration -d and -f\n");
                            return;
                        }
                        break;
                    case 'f':
                        only_files_flag = true;
                        if (only_directories_flag) {
                            print_error("Error: seek: Invalid configuration -d and -f\n");
                            return;
                        }
                        break;
                    case 'e':
                        output_flag = true;
                        break;
                    default:
                        print_error("Error: seek: Unrecognized option -");
                        char unrecognized_flag[2] = "\0\0";
                        unrecognized_flag[0] = *flag_pointer;
                        print_error(unrecognized_flag);
                        print_error("\n");
                        return;
                }
            }
        } else if (target == NULL) {
            target = argv[i];
        } else if (!custom_search_directory) {
            search_directory = argv[i];
            custom_search_directory = true;
        } else {
            print_error("Error: seek: Too many arguments\n");
            return;
        }
    }
    if (target == NULL) {
        printf("Error: seek: Required search target\n");
        return;
    }
    bool found_one = false;
    char *last_path = NULL;
    search(target, search_directory, only_directories_flag, only_files_flag, &found_one, &last_path, search_directory);

    if (found_one && output_flag) {
        struct stat path_stat;
        stat(last_path, &path_stat);
        if (S_ISDIR(path_stat.st_mode)) {
            char *previous_working_directory = getcwd(NULL, 0);
            if (chdir(last_path) == -1) {
                print_errno_error("Couldn't change directory: %s\n");
                free(previous_working_directory);
            } else {
                if (global_config.previous_working_directory != NULL)
                    free(global_config.previous_working_directory);
                global_config.previous_working_directory = previous_working_directory;
            }
        } else {
            FILE *file = fopen(last_path, "r");
            if (file == NULL) {
                print_errno_error("Couldn't open file for reading: %s\n");
            } else {
                char c;
                while ((c = fgetc(file)) != EOF)
                    putchar(c);
                printf("\n");
                fclose(file);
            }

        }

        free(last_path);
    } else if (!found_one) {
        print_error("No Matches\n");
    }
}
