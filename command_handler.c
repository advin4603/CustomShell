#include "headers.h"

typedef struct BackgroundProcess {
    pid_t pid;
    char *process_name;
} BackgroundProcess;

LinkedList background_processes = {.head = NULL, .size = 0};

bool filter_background_processes(BackgroundProcess *background_process) {
    int exit_status;
    if (waitpid(background_process->pid, &exit_status, WNOHANG) != 0) {
        printf("%s exited %s (%d)\n", background_process->process_name,
               WIFEXITED(exit_status) ? "normally" : "abnormally", background_process->pid);
        free(background_process->process_name);
        free(background_process);
        return false;
    }
    return true;
}

void pre_parsed_input_handler(char *pre_parsed_input, bool register_command) {
    linked_list_filter(&background_processes, (bool (*)(void *)) filter_background_processes);
    bool has_pastevents_execute = false;
    if (syntax_check(pre_parsed_input)) {
        ParseResult parse_result = parse(pre_parsed_input);
        while (parse_result.kind != PARSE_END) {
            Command *parsed_command = parse_result.command;
            bool command_contains_pastevents_execute = strcmp(parsed_command->arguments[0], "pastevents") == 0 &&
                                                       parsed_command->argument_count > 1 &&
                                                       strcmp(parsed_command->arguments[1], "execute") == 0;
            has_pastevents_execute = has_pastevents_execute || command_contains_pastevents_execute;
            register_command = register_command && (strcmp(parsed_command->arguments[0], "pastevents") != 0 ||
                                                    (command_contains_pastevents_execute));
            command_handler(parsed_command);
            parse_result = parse(NULL);
        }
    }

    if (register_command)
        register_command_in_pastevents(pre_parsed_input, has_pastevents_execute);
}

void command_handler(Command *command) {
    char *command_name = command->arguments[0];
    if (strcmp(command_name, "warp") == 0) {
        warp(command->argument_count, command->arguments);
    } else if (strcmp(command_name, "clear") == 0) {
        CLEAR_SCREEN();
    } else if (strcmp(command_name, "peek") == 0) {
        peek(command->argument_count, command->arguments);
    } else if (strcmp(command_name, "pastevents") == 0) {
        pastevents(command->argument_count, command->arguments);
    } else if (strcmp(command_name, "proclore") == 0) {
        proclore(command->argument_count, command->arguments);
    } else if (strcmp(command_name, "seek") == 0) {
        seek(command->argument_count, command->arguments);
    } else {
        pid_t pid = fork();
        if (pid == -1) {
            print_errno_error("Error while forking: %s\n");
        } else if (pid == 0) {
            // child
            char **null_terminated_arguments = malloc((command->argument_count + 1) * sizeof(char *));
            null_terminated_arguments[command->argument_count] = NULL;
            memcpy(null_terminated_arguments, command->arguments, command->argument_count * sizeof(char *));
            printf(ANSI_COLOR_RESET);
            fflush(stdout);
            int status_code = execvp(null_terminated_arguments[0], null_terminated_arguments);
            if (status_code == -1) {
                if (errno == ENOENT) {
                    print_errno_error(command->arguments[0]);
                    print_errno_error(": command not found\n");
                } else {
                    print_errno_error("Error: %s\n");
                }
                exit(-1);
            }
            exit(0);
        } else {
            if (command->is_foreground) {
                int exit_status;
                unsigned long long t = time(NULL);
                wait(&exit_status);
                global_config.time_taken_by_previous_command = (time(NULL) - t);
                global_config.previous_command_name = string_duplicate(command_name);
            } else {
                printf("%d\n", pid);
                BackgroundProcess *new_background_process = malloc(sizeof(BackgroundProcess));
                new_background_process->pid = pid;
                new_background_process->process_name = string_duplicate(command_name);
                linked_list_push(&background_processes, new_background_process);
            }

        }
    }


    free_command_space(command);
}

void free_command_space(Command *command) {
    for (size_t i = 0; i < command->argument_count; i++)
        free(command->arguments[i]);
    free(command->arguments);
    free(command);
}