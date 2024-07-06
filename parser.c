#include "headers.h"

// TODO escaping newline doesnt include in command?

PreParseCheckResult pre_parse_check(char *input) {
    bool escape_next = false, inside_string = false;
    char string_terminator = '\0';
    size_t index;
    for (index = 0; *input != '\0'; input++, index++) {
        switch (*input) {
            case '\n':
                if ((escape_next || inside_string) && *(input + 1) == '\0') {
                    if (index < MAX_INPUT_LENGTH)
                        return (PreParseCheckResult) {.kind = CONTINUE_READING, .characters_scanned = index + 1};
                    return (PreParseCheckResult) {.kind = LENGTH_EXCEEDED, .characters_scanned = index + 1};
                }
                escape_next = false;
                break;
            case '\'':
            case '"':
                if (!escape_next) {
                    if (inside_string) {
                        if (string_terminator == *input) {
                            inside_string = false;
                        }
                    } else {
                        string_terminator = *input;
                        inside_string = true;
                    }

                }
                escape_next = false;
                break;
            case '\\':
                escape_next = !escape_next;
                break;
            default:
                escape_next = false;
                break;

        }

    }
    if (index >= MAX_INPUT_LENGTH)
        return (PreParseCheckResult) {.kind = LENGTH_EXCEEDED, .characters_scanned = index + 1};

    return (PreParseCheckResult) {.kind =  SUCCESS, .characters_scanned=index};
}

bool syntax_check(char *input) {
    ParseResult parse_result = parse(input);
    while (parse_result.kind != PARSE_END) {
        if (parse_result.kind == PARSE_ERROR)
            return false;
        parse_result = parse(NULL);
    }
    return true;
}

ParseResult parse(char *input_string) {
    static char *string_start = NULL;
    if (input_string != NULL) {
        string_start = input_string;
    }

    if (string_start == NULL || *string_start == '\0')
        return (ParseResult) {.kind = PARSE_END, .command = NULL, .start_pointer = NULL, .end_pointer = NULL};

    bool escape_next = false, inside_string = false;
    char string_terminator = '\0';
    size_t home_directory_path_length = strlen(global_config.home_directory_path);
    char arg_buffer[MAX_INPUT_LENGTH +
                    home_directory_path_length]; // an argument can only be as big as the max input length + one ~ substitution
    char *args_buffer[MAX_INPUT_LENGTH]; // a Command can only have argument count strictly less than max input length
    char *arg_pointer = arg_buffer;
    char **args_pointer = args_buffer;
    char *command_start_pointer = input_string;
    for (; *string_start != '\0'; string_start++) {
        switch (*string_start) {
            case ' ':
            case '\n':
            case '\t':
                if (escape_next || inside_string) {
                    *(arg_pointer++) = *string_start;
                    escape_next = false;
                    break;
                }
                if (arg_pointer != arg_buffer) {
                    size_t arg_length = arg_pointer - arg_buffer;
                    char *arg = malloc(arg_length + 1);
                    memcpy(arg, arg_buffer, arg_length);
                    arg[arg_length] = '\0';
                    *(args_pointer++) = arg;
                    arg_pointer = arg_buffer;

                }
                break;
            case '~':
                if (escape_next || inside_string || arg_pointer != arg_buffer) {
                    *(arg_pointer++) = *string_start;
                    escape_next = false;
                    break;
                }
                memcpy(arg_pointer, global_config.home_directory_path, home_directory_path_length);
                arg_pointer += home_directory_path_length;
                break;
            case '&':
            case ';':
                if (escape_next || inside_string) {
                    *(arg_pointer++) = *string_start;
                    escape_next = false;
                    break;
                }
                if (arg_pointer != arg_buffer) {
                    size_t arg_length = arg_pointer - arg_buffer;
                    char *arg = malloc(arg_length + 1);
                    memcpy(arg, arg_buffer, arg_length);
                    arg[arg_length] = '\0';
                    *(args_pointer++) = arg;
                } else if (args_pointer == args_buffer) {
                    print_error("Syntax Error: Unexpected ");
                    char unexpected_character[2];
                    unexpected_character[0] = *string_start;
                    unexpected_character[1] = '\0';
                    print_error(unexpected_character);
                    print_error("\n");
                    return (ParseResult) {.kind = PARSE_ERROR, .command = NULL, .start_pointer = NULL, .end_pointer = NULL};
                }
                Command *parsed_command = malloc(sizeof(Command));
                parsed_command->argument_count = args_pointer - args_buffer;
                parsed_command->arguments = malloc(parsed_command->argument_count * sizeof(char *));
                parsed_command->is_foreground = *string_start == ';';
                memcpy(parsed_command->arguments, args_buffer, parsed_command->argument_count * sizeof(char *));
                string_start++; // advance to after ; or & for next Command in string.

                return (ParseResult) {.kind = PARSED, .command = parsed_command, .start_pointer = command_start_pointer, .end_pointer =
                string_start - 2 /* go to before ; or & */};
            case '\'':
            case '"':
                if (!escape_next) {
                    if (inside_string) {
                        if (string_terminator == *string_start) {
                            inside_string = false;
                        } else {
                            *(arg_pointer++) = *string_start;
                        }
                    } else {
                        string_terminator = *string_start;
                        inside_string = true;
                    }

                } else {
                    *(arg_pointer++) = *string_start;
                }
                escape_next = false;
                break;
            case '\\':
                if (escape_next) {
                    *(arg_pointer++) = *string_start;
                    escape_next = false;
                } else
                    escape_next = true;

                break;
            default:
                escape_next = false;
                *(arg_pointer++) = *string_start;
                break;
        }
    }
    if (args_pointer == args_buffer) {
        return (ParseResult) {.kind = PARSE_END, .command = NULL, .start_pointer = NULL, .end_pointer = NULL};
    }
    Command *parsed_command = malloc(sizeof(Command));
    parsed_command->argument_count = args_pointer - args_buffer;
    parsed_command->arguments = malloc(parsed_command->argument_count * sizeof(char *));
    parsed_command->is_foreground = true;
    memcpy(parsed_command->arguments, args_buffer, parsed_command->argument_count * sizeof(char *));

    return (ParseResult) {.kind = PARSED, .command = parsed_command, .start_pointer = command_start_pointer, .end_pointer =
    string_start - 1 /* go before the \0 */};
}