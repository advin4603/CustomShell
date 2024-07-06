#include "headers.h"


bool starts_with(const char *string, const char *prefix) {
    return strncmp(string, prefix, strlen(prefix)) == 0;
}

char *string_duplicate(const char *source) {
    size_t source_length = strlen(source);
    char *destination = malloc(source_length + 1);
    destination[source_length] = '\0';
    if (destination == NULL) return NULL;
    memcpy(destination, source, source_length);
    return destination;
}

char *string_replace_between(char *source, char *left, char*right, char* replace) {
    size_t source_length = strlen(source);
    char *replaced = malloc( source_length - (right - left) + strlen(replace) + 1);
    char *source_pointer = source;
    char *replaced_pointer = replaced;
    while (*source_pointer && source_pointer != left )
        *(replaced_pointer++) = *(source_pointer++);
//
    while (*replace)
        *(replaced_pointer++) = *(replace++);
    while (*source_pointer && source_pointer != right) source_pointer++;
    while (*source_pointer)
        *(replaced_pointer++) = *(source_pointer++);
    *(replaced_pointer++) = '\0';

    return replaced;
}
