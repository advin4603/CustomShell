#ifndef PARSER_H_
#define PARSER_H_

typedef struct Command {
    char **arguments;
    size_t argument_count;
    bool is_foreground;
} Command;


enum PreParseCheckResultKind {
    SUCCESS, LENGTH_EXCEEDED, CONTINUE_READING
};

typedef struct PreParseCheckResult {
    enum PreParseCheckResultKind kind;
    size_t characters_scanned;
} PreParseCheckResult;

enum ParseResultKind {
    PARSED, PARSE_END, PARSE_ERROR
};

typedef struct ParseResult {
    enum ParseResultKind kind;
    Command *command;
    char *start_pointer;
    char *end_pointer;
} ParseResult;

PreParseCheckResult pre_parse_check(char *input);

ParseResult parse(char *input_string);

bool syntax_check(char *input);
#endif // PARSER_H_
