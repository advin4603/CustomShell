#ifndef ERRORS_H
#define ERRORS_H

extern int errno;

void print_error(char *error_string);
void print_errno_error(const char *error_format);
#endif // ERRORS_H
