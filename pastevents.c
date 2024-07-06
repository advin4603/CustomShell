#include "headers.h"

typedef struct PasteventsData {
    char *commands[PASTEVENTS_COMMAND_COUNT];
    size_t command_count;
    size_t oldest_command_index;
} PasteventsData;

PasteventsData pastevents_data = {.command_count = 0};


char *get_command_indexed_from_oldest(size_t index) {
    if (index >= pastevents_data.command_count)
        return NULL;
    return pastevents_data.commands[(pastevents_data.oldest_command_index + index) % PASTEVENTS_COMMAND_COUNT];
}

char *get_command_indexed_from_newest(size_t index) {
    if (index >= pastevents_data.command_count)
        return NULL;
    index = (pastevents_data.command_count - index - 1);
    return get_command_indexed_from_oldest(index);
}

char *get_pastevents_save_file_path() {
    char *username = getlogin();
    char *file_path = malloc(strlen("/home/") + strlen(username) + strlen(PASTEVENTS_SAVE_FILE_NAME) + 1);
    sprintf(file_path, "/home/%s/"PASTEVENTS_SAVE_FILE_NAME, username);

    return file_path;
};

void pastevents_store() {
    char *file_path = get_pastevents_save_file_path();
    FILE *pastevents_file_pointer = fopen(file_path, "w");
    free(file_path);

    if (pastevents_file_pointer == NULL) {
        print_errno_error("Error: Couldn't open .pastevents: %s");
        return;
    }

    for (size_t index = 0; index < pastevents_data.command_count; index++) {
        char *pastevent = get_command_indexed_from_oldest(index);
        fputs(pastevent, pastevents_file_pointer);
        fputc('\0', pastevents_file_pointer);
    }

    fclose(pastevents_file_pointer);

}

void push_command_in_pastevents(const char *command) {
    if (pastevents_data.command_count >= PASTEVENTS_COMMAND_COUNT) {
        free(pastevents_data.commands[pastevents_data.oldest_command_index]);
        pastevents_data.oldest_command_index++;
        pastevents_data.oldest_command_index %= PASTEVENTS_COMMAND_COUNT;
        pastevents_data.command_count--;
    }
    size_t push_index =
            (pastevents_data.oldest_command_index + pastevents_data.command_count) % PASTEVENTS_COMMAND_COUNT;
    size_t command_length = strlen(command);
    pastevents_data.commands[push_index] = malloc(command_length + 1);
    strcpy(pastevents_data.commands[push_index], command);
    pastevents_data.command_count++;

}

void pastevents_load() {
    char *file_path = get_pastevents_save_file_path();
    FILE *pastevents_file_pointer = fopen(file_path, "r");
    if (pastevents_file_pointer == NULL) {
        if (errno == ENOENT) {
            return; // no file to load
        }
        print_errno_error("Error: Couldn't load .pastevents\n");
        return;
    }

    fseek(pastevents_file_pointer, 0, SEEK_END);
    size_t file_size = ftell(pastevents_file_pointer);
    fseek(pastevents_file_pointer, 0, SEEK_SET);
    char *commands = malloc(file_size);
    fread(commands, 1, file_size, pastevents_file_pointer);

    char *command = malloc(file_size); // command can be as big as the file_size
    char *command_head = command;
    for (size_t i = 0; i < file_size; i++) {
        switch (commands[i]) {
            case '\0':
                *(command_head++) = commands[i];
                push_command_in_pastevents(command);
                command_head = command; // reset command head
                break;
            default:
                *(command_head++) = commands[i];
                break;

        }
    }
    free(commands);
    free(command);

}

void pastevents_config() {
    pastevents_load();
}

void register_command_in_pastevents(char *command, bool has_pastevents_execute) {
    bool free_command = false;
    if (has_pastevents_execute) {
        bool do_parsing = true;
        bool do_store = true;
        while (do_parsing) {
            ParseResult parse_result = parse(command);
            while (parse_result.kind != PARSE_END) {
                if (strcmp(parse_result.command->arguments[0], "pastevents") == 0 &&
                    parse_result.command->argument_count == 3 &&
                    strcmp(parse_result.command->arguments[1], "execute") == 0) {
                    char *argument = parse_result.command->arguments[2];
                    char *end;
                    size_t index = strtoul(argument, &end, 10);
                    size_t num_length = snprintf(NULL, 0, "%lu", index);
                    if (strlen(argument) != num_length || end == argument || errno == ERANGE ||
                        index > pastevents_data.command_count) {
                        parse_result = parse(NULL);
                        if (parse_result.kind == PARSE_END) do_parsing = false;
                        do_store = false;
                        continue;
                    }


                    char *past_command = get_command_indexed_from_newest(index - 1);
                    if (past_command == NULL) {
                        parse_result = parse(NULL);
                        if (parse_result.kind == PARSE_END) do_parsing = false;
                        do_store = false;
                        continue;
                    }
                    free_command = true;
                    if (*(parse_result.end_pointer) == '\n')
                        parse_result.end_pointer++;
                    command = string_replace_between(command, parse_result.start_pointer, parse_result.end_pointer,
                                                     past_command);
                    free_command_space(parse_result.command);
                    break;
                }

                free_command_space(parse_result.command);
                parse_result = parse(NULL);
                if (parse_result.kind == PARSE_END) do_parsing = false;
            }
        }
        if (!do_store)
            return;

    }

    char *previous_command = get_command_indexed_from_newest(0);
    if (previous_command != NULL && strcmp(command, previous_command) == 0) {
        return;
    }

    push_command_in_pastevents(command);
    if (free_command) free(command);
    pastevents_store();
}

void pastevents_list() {
    for (size_t i = 0; i < pastevents_data.command_count; i++) {
        printf("%s", get_command_indexed_from_oldest(i));
    }
}

void pastevents_purge() {
    for (size_t i = 0; i < pastevents_data.command_count; i++) {
        free(get_command_indexed_from_oldest(i));
    }
    pastevents_data.command_count = 0;
    pastevents_data.oldest_command_index = 0;
    pastevents_store();
}


void pastevents_execute(char *argument) {
    char *end;
    size_t index = strtoul(argument, &end, 10);
    size_t num_length = snprintf(NULL, 0, "%lu", index);
    if (strlen(argument) != num_length) {
        print_error("pastevents: execute: numeric argument required\n");
        return;
    }
    if (end == argument) {
        print_error("pastevents: execute: numeric argument required\n");
        return;
    } else if (errno == ERANGE || index > pastevents_data.command_count) {
        print_error("pastevents: execute: argument not in range of available commands\n");
        return;
    }
    char *past_command = get_command_indexed_from_newest(index - 1);
    if (past_command == NULL) {
        print_error("pastevents: execute: couldn't get command");
        return;
    }
    pre_parsed_input_handler(past_command, false);
}

void pastevents(size_t argc, char **argv) {
    if (argc == 1)
        pastevents_list();
    else if (argc == 2 && strcmp(argv[1], "purge") == 0)
        pastevents_purge();
    else if (argc == 3 && strcmp(argv[1], "execute") == 0)
        pastevents_execute(argv[2]);
    else {
        print_error("pastevents:");
        for (size_t i = 1; i < argc; i++) {
            print_error(" ");
            print_error(argv[i]);
        }
        print_error(": Invalid arguments\n");
    }


}
