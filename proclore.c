#include "headers.h"


void proclore(size_t argc, char **argv) {
    char *pid = argv[1];
    bool free_pid = false;
    bool is_foreground = false;
    if (argc == 1) {
        free_pid = is_foreground = true;
        pid_t pid_number = getpid();
        pid = malloc(PID_MAX + 1);
        sprintf(pid, "%d", pid_number); // TODO maybe use snprintf to get length then allocate
    }

    char *stat_path = malloc(strlen("/proc") + strlen(pid) + strlen("/stat") + 1);
    char *exe_path = malloc(strlen("/proc") + strlen(pid) + strlen("/exe") + 1);
    sprintf(stat_path, "/proc/%s/stat", pid);
    sprintf(exe_path, "/proc/%s/exe", pid);

    FILE *proc_stat_file = fopen(stat_path, "r");
    if (proc_stat_file == NULL) {
        print_errno_error("Couldn't find process: %s\n");
        return;
    }


    char state[3] = "\0\0\0";
    size_t virtual_memory;
    fscanf(proc_stat_file,
           "%*d %*s %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*d %*d %*d %*d %*d %*d %*u %lu",
           state, &virtual_memory);

    char *exe_path_resolved = realpath(exe_path, NULL);
    if (exe_path_resolved == NULL) {
        exe_path_resolved = "?";
    }
    if (is_foreground) { state[1] = '+'; }

    printf("pid: %s\nprocess status: %s\nprocess group: %d\nVirtual Memory: %lu\nExecutable Path: %s\n", pid, state,
           getpgid(atoi(pid)),
           virtual_memory, exe_path_resolved);


    free(stat_path);
    free(exe_path);
    fclose(proc_stat_file);
    if (free_pid)
        free(pid);
}