#include "headers.h"


char *get_pre_parse_checked_input(char *input_buffer) {
    size_t characters_scanned = 0;

    bool do_pre_parse_check = true;
    bool pre_parse_check_passed = false;
    while (do_pre_parse_check) {
        printf(ANSI_COLOR_GREEN);
        fgets(input_buffer + characters_scanned, MAX_INPUT_LENGTH + 1 - characters_scanned, stdin);
        printf(ANSI_COLOR_RESET);
        PreParseCheckResult result = pre_parse_check(input_buffer);
        switch (result.kind) {
            case SUCCESS:
                do_pre_parse_check = false;
                pre_parse_check_passed = true;
                break;
            case LENGTH_EXCEEDED:
                do_pre_parse_check = false;
                print_error("Command exceeds max Command length\n");
                int c;
                if (input_buffer[MAX_INPUT_LENGTH - 1] != '\n') // if stdin not exhausted
                    while ((c = getchar()) != '\n' && c != EOF);
                break;
            case CONTINUE_READING:
                characters_scanned = result.characters_scanned;
                printf(ANSI_COLOR_MAGENTA"⟶  "ANSI_COLOR_RESET);
                break;
        }

    }

    if (pre_parse_check_passed)
        return input_buffer;

    return NULL;
}