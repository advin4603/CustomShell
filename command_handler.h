#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

void command_handler(Command *);
void free_command_space(Command *);
void pre_parsed_input_handler(char *pre_parsed_input, bool register_command);

#endif // COMMAND_HANDLER_H
