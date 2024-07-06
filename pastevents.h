#ifndef HISTORY_H
#define HISTORY_H

void register_command_in_pastevents(char *command, bool has_pastevents_execute);

void pastevents(size_t argc, char **argv);

void pastevents_config();

#endif // HISTORY_H
