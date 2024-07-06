#include "headers.h"

int no_hidden_filter(const struct dirent *entry) {
    return entry->d_name[0] != '.';
}

char get_file_type_letter(mode_t mode) {

    if (S_ISREG(mode))
        return '-';
    else if (S_ISDIR(mode))
        return 'd';
    else if (S_ISBLK(mode))
        return 'b';
    else if (S_ISCHR(mode))
        return 'c';
#ifdef S_ISFIFO
    else if (S_ISFIFO(mode))
        return 'p';
#endif  /* S_ISFIFO */
#ifdef S_ISLNK
    else if (S_ISLNK(mode))
        return 'l';
#endif  /* S_ISLNK */
#ifdef S_ISSOCK
    else if (S_ISSOCK(mode))
        return 's';
#endif  /* S_ISSOCK */
#ifdef S_ISDOOR
        /* Solaris 2.6, etc. */
    else if (S_ISDOOR(mode))
        return 'D';
#endif  /* S_ISDOOR */
    else {
        /* Unknown type -- possibly a regular file? */
        return '?';
    }
}

char *permissions_formatter(mode_t mode) {
    char rwx[][4] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};
    char *bits = malloc(1 + 3 + 3 + 3 + 1);
    bits[0] = get_file_type_letter(mode);
    memcpy(&bits[1], rwx[(mode >> 6) & 0b111], 3);
    memcpy(&bits[4], rwx[(mode >> 3) & 0b111], 3);
    memcpy(&bits[7], rwx[mode & 0b111], 3);
    bits[10] = '\0';
    return bits;
}


void list_directories(bool show_parent_name, bool all_flag, bool long_listing_flag, char *path) {
    struct dirent **directory_entries;
    int directory_entry_count = scandir(path, &directory_entries, all_flag ? NULL : no_hidden_filter, alphasort);
    if (show_parent_name)
        printf(ANSI_COLOR_YELLOW"%s:\n"ANSI_COLOR_RESET, path);
    if (directory_entry_count == -1) {
        print_errno_error("Could not open directory: %s\n");
        return;
    }

    if (long_listing_flag) {
        blkcnt_t total_block_count = 0;
        for (int i = 0; i < directory_entry_count; i++) {
            struct dirent *directory_entry = directory_entries[i];
            struct stat stat_buffer;
            char *file_path = join_paths(path, directory_entry->d_name);
            if (lstat(file_path, &stat_buffer) == -1) {
                print_errno_error("Couldn't stat: %s\n");
                return;
            }
            free(file_path);
            total_block_count += stat_buffer.st_blocks;
        }
        printf("total %ld\n", total_block_count / 2);
        // Why divide by 2?
        // https://askubuntu.com/questions/1252657/why-is-total-stat-block-size-of-a-directory-twice-of-ls-block-size-of-a-director
    }


    for (int i = 0; i < directory_entry_count; i++) {
        struct dirent *directory_entry = directory_entries[i];
        struct stat stat_buffer;
        char *file_path = join_paths(path, directory_entry->d_name);
        if (lstat(file_path, &stat_buffer) == -1) {
            print_errno_error("Couldn't stat: %s\n");
            return;
        }
        free(file_path);
        if (long_listing_flag) {
            char *permission_string = permissions_formatter(stat_buffer.st_mode);
            printf("%s ", permission_string);
            free(permission_string);
            printf("%3lu ", stat_buffer.st_nlink);
            struct passwd *user_info = getpwuid(stat_buffer.st_uid);
            if (user_info == NULL) {
                print_errno_error("Couldn't get user name: %s\n");
                return;
            }
            printf("%10s ", user_info->pw_name);
            struct group *group_info = getgrgid(stat_buffer.st_gid);
            if (group_info == NULL) {
                print_errno_error("Couldn't get group name: %s\n");
                return;
            }
            printf("%10s ", group_info->gr_name);
            printf("%10ld ", stat_buffer.st_size);
            struct tm current_time, file_time;
            time_t current_time_time_t = time(NULL);
            localtime_r(&current_time_time_t, &current_time);
            localtime_r(&stat_buffer.st_mtime, &file_time);

            char time_string_buffer[100];
            strftime(time_string_buffer, 100, file_time.tm_year == current_time.tm_year ? "%b %d %H:%M" : "%b %d  %Y",
                     localtime(&stat_buffer.st_mtime));
            printf("%s ", time_string_buffer);
        }

        switch (directory_entry->d_type) {
            case DT_DIR:
                printf(ANSI_COLOR_BLUE);
                break;
            case DT_LNK:
                printf(ANSI_COLOR_CYAN);
                break;
            default:
                if (stat_buffer.st_mode & X_OK) {
                    printf(ANSI_COLOR_GREEN);
                }
                break;

        }


        printf("%s\n", directory_entry->d_name);
        printf(ANSI_COLOR_RESET);
        free(directory_entry);
    }
    printf("\n");
    if (show_parent_name)
        printf("\n"); // print extra newline

    free(directory_entries);
}

void peek(size_t argc, char **argv) {
    bool all_flag = false;
    bool long_listing_flag = false;
    size_t paths = 0;
    for (size_t i = 1; i < argc; i++) {
        char *argument = argv[i];
        if (*argument == '-')
            while (*++argument) {
                switch (*argument) {
                    case 'a':
                        all_flag = true;
                        break;
                    case 'l':
                        long_listing_flag = true;
                        break;
                    default:
                        print_error("Invalid option: ");
                        char wrong_flag[2] = "\0\0";
                        wrong_flag[0] = *argument;
                        print_error(wrong_flag);
                        print_error("\n");
                        return;

                }
            }
        else
            paths++;
    }
    if (paths == 0) {
        list_directories(false, all_flag, long_listing_flag, ".");
        return;
    }
    for (size_t i = 1; i < argc; i++) {
        char *argument = argv[i];
        if (*argument == '-')
            continue;
        list_directories(paths > 1, all_flag, long_listing_flag, argument);
    }
}