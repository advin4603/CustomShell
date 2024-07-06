#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define MAX_INPUT_LENGTH 4096

#define PASTEVENTS_COMMAND_COUNT 15
#define PASTEVENTS_SAVE_FILE_NAME ".pastevents"
#define PID_MAX 99999

#define CLEAR_SCREEN() printf("\e[1;1H\e[2J");