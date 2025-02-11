#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    clist->num = 0;

    // Split command line into individual commands based on pipe characters
    char *token = strtok(cmd_line, PIPE_STRING);
    while (token != NULL && clist->num < CMD_MAX) {
        while (*token == SPACE_CHAR) { // trim any leading spaces
            token++;
        }

        // trim trailing spaces
        char *end = token + strlen(token) - 1;
        while (end > token && *end == SPACE_CHAR) {
            end--;
        }

        *(end + 1) = '\0'; // null terminate trimmed command

        // check if command is too long
        if (strlen(token) >= SH_CMD_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        // split command into executable name and arguments
        clist->commands[clist->num].args[0] = '\0'; // initialize arguments to be emptied
        char *arg = strtok(token, " "); // split spaces 
        if (arg != NULL) {
            // check if executable name is too long
            if (strlen(arg) >= EXE_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }

            strncpy(clist->commands[clist->num].exe, arg, EXE_MAX - 1); // copy executable name into command structure
            clist->commands[clist->num].exe[EXE_MAX - 1] = '\0'; // ensure there is null termination

            // remaining part of command is arguments, doing same as we did with executable
            arg = strtok(NULL, "");
            if (arg != NULL) {
                // check if args are too long
                if (strlen(arg) >= ARG_MAX) {
                    return ERR_CMD_OR_ARGS_TOO_BIG
                }

                strncpy(clist->commands[clist->num].exe, arg, ARG_MAX - 1);
                clist->commands[clist->num].exe[ARG_MAX - 1] = '\0';
            }
        }

        clist->num++; // increase command count

        token = strtok(NULL, PIPE_STRING); // get next command

    }

    // checking if there were too many commands and printign error if necessary
    if (clist->num >= CMD_MAX) {
        return ERR_TOO_MANY_COMMANDS;
    }

    return OK; // otherwise success
}