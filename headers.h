#ifndef HEADERS_H_
#define HEADERS_H_

#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/wait.h>

#include "shell_config.h"
#include "prompt.h"
#include "constants.h"
#include "parser.h"
#include "errors.h"
#include "input.h"
#include "command_handler.h"
#include "warp.h"
#include "peek.h"
#include "pastevents.h"
#include "string_utilities.h"
#include "proclore.h"
#include "seek.h"
#include "path_utilities.h"
#include "linked_list.h"

#endif