#include "headers.h"


char *join_paths(const char *path_1, const char *path_2) {
    size_t path_1_length = strlen(path_1);
    size_t path_2_length = strlen(path_2);
    bool ends_with_separator = path_1[path_1_length - 1] == '/';
    char *final_path = malloc(path_1_length + path_2_length + (ends_with_separator ? 0 : 1) + 1);
    memcpy(final_path, path_1, path_1_length + 1);
    if (ends_with_separator) {
        memcpy(final_path + path_1_length, path_2, path_2_length + 1);
    } else {
        final_path[path_1_length] = '/';
        memcpy(final_path + path_1_length + 1, path_2, path_2_length + 1);
    }

    return final_path;
}