#ifndef SHELL_CONFIG_H
#define SHELL_CONFIG_H

typedef struct GlobalConfig {
    char *home_directory_path;
    char *previous_working_directory;
    char *previous_command_name;
    unsigned long long time_taken_by_previous_command;
} GlobalConfig;

extern GlobalConfig global_config;

#endif // SHELL_CONFIG_H
