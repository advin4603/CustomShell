#include "headers.h"

void print_error(char *error_string) {
    fprintf(stderr, ANSI_COLOR_RED"%s"ANSI_COLOR_RESET, error_string);
}

void print_errno_error(const char *error_format)
{
    const char *error_reason = strerror(errno);
    char *error_message = (char *)malloc(strlen(error_format) - 2 /* Remove space for "%s" */ + strlen(error_reason) + 1 /* Add Space for \0 */);
    if (error_message == NULL)
    {
        perror("error"); // In case malloc fails, print error using perror and return
        return;
    }
    sprintf(error_message, error_format, error_reason);
    print_error(error_message);
    free(error_message);
}