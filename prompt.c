#include "headers.h"

char *get_formatted_working_directory() {
    char *home_directory = global_config.home_directory_path;
    char *current_working_directory = getcwd(NULL, 0);
    if (!starts_with(current_working_directory, home_directory)) {
        return current_working_directory;
    }

    char *formatted_current_working_directory =
            current_working_directory + strlen(home_directory) - 1 /* Keep space for ~ */;
    *formatted_current_working_directory = '~';
    formatted_current_working_directory = string_duplicate(formatted_current_working_directory);
    free(current_working_directory); // free no longer required memory allocated by getcwd
    return formatted_current_working_directory;
}

void prompt() {
    char *username = getlogin();
    char system_name[HOST_NAME_MAX + 1];
    gethostname(system_name, HOST_NAME_MAX);
    char *working_directory = get_formatted_working_directory();
    if (global_config.previous_command_name == NULL || global_config.time_taken_by_previous_command < 2)
        printf(ANSI_COLOR_MAGENTA"<"
               ANSI_COLOR_CYAN"%s"
               ANSI_COLOR_MAGENTA"@"
               ANSI_COLOR_YELLOW"%s"
               ANSI_COLOR_MAGENTA":"
               ANSI_COLOR_BLUE"%s"
               ANSI_COLOR_MAGENTA"> "
               ANSI_COLOR_RESET, username, system_name, working_directory);
    else
        printf(ANSI_COLOR_MAGENTA"<"
               ANSI_COLOR_CYAN"%s"
               ANSI_COLOR_MAGENTA"@"
               ANSI_COLOR_YELLOW"%s"
               ANSI_COLOR_MAGENTA":"
               ANSI_COLOR_BLUE"%s"
               ANSI_COLOR_RED" %s took %llds "
               ANSI_COLOR_MAGENTA"> "
               ANSI_COLOR_RESET, username, system_name, working_directory, global_config.previous_command_name,
               global_config.time_taken_by_previous_command);
    if (global_config.previous_command_name != NULL) {
        free(global_config.previous_command_name);
        global_config.previous_command_name = NULL;
    }
    global_config.time_taken_by_previous_command = -1;
    free(working_directory);
}
