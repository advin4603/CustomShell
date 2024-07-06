#include "headers.h"

GlobalConfig global_config = {.home_directory_path = NULL, .previous_working_directory = NULL, .previous_command_name = NULL, .time_taken_by_previous_command = 0};

void start_config() {
    CLEAR_SCREEN();
    char *home_directory_path = getcwd(NULL, 0);
    global_config.home_directory_path = home_directory_path;
    pastevents_config();
}

int main() {
    start_config();
    // Keep accepting commands
    while (1) {
        // Print appropriate prompt with username, system name and directory before accepting input
        prompt();
        char input[MAX_INPUT_LENGTH + 1];

        char *pre_parsed_input = get_pre_parse_checked_input(input);

        if (pre_parsed_input == NULL) continue;

        pre_parsed_input_handler(pre_parsed_input, true);
        fflush(stdout);
    }
}
