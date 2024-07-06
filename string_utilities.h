#ifndef STRING_UTILITIES_H
#define STRING_UTILITIES_H

bool starts_with(const char *string, const char *prefix) ;

char *string_duplicate(const char *source);
char *string_replace_between(char *source, char *left, char*right, char* replace);

#endif // STRING_UTILITIES_H
