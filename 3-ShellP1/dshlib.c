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
 int build_cmd_list(char *cmd_line, command_list_t *clist) {
    clist->num = 0; // number of commands is initially 0
    char *cmd_token;
    char *arg_token;
    char *saveptr1, *saveptr2;

    // Trim leading and trailing spaces from the input command line
    while (*cmd_line == SPACE_CHAR) {
        cmd_line++;
    }

    char *end = cmd_line + strlen(cmd_line) - 1;
    while (end > cmd_line && *end == SPACE_CHAR) {
        end--;
    }
    *(end + 1) = '\0'; // null terminate string

    // Count the number of piped commands
    char *temp_line = strdup(cmd_line); // duplicating since not doing this caused issues with piped commands
    if (!temp_line) {
        return ERR_CMD_OR_ARGS_TOO_BIG; // memory allocation failed
    }

    int temp_cmd_count = 0;
    // count number of commands by looping through tokens and incrementing
    for (cmd_token = strtok_r(temp_line, PIPE_STRING, &saveptr1); cmd_token != NULL; cmd_token = strtok_r(NULL, PIPE_STRING, &saveptr1)) {
        temp_cmd_count++;
    }
    free(temp_line);

    if (temp_cmd_count > CMD_MAX) { // returns error if max command count is exceeded
        return ERR_TOO_MANY_COMMANDS;
    }

    // Tokenize commands and their arguments after getting count
    int cmd_index = 0;
    for (cmd_token = strtok_r(cmd_line, PIPE_STRING, &saveptr1); cmd_token != NULL; cmd_token = strtok_r(NULL, PIPE_STRING, &saveptr1)) {
        if (cmd_index >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS; // making sure command count isn't exceeded 
        }
        
        // Trim leading and trailing spaces again for each command
        while (*cmd_token == SPACE_CHAR) {
            cmd_token++;
        }
        end = cmd_token + strlen(cmd_token) - 1;
        while (end > cmd_token && *end == SPACE_CHAR) {
            end--;
        }
        *(end + 1) = '\0';

        command_t *cmd = &clist->commands[cmd_index];
        memset(cmd, 0, sizeof(command_t)); // initialize size of command structure

        // Extract the first word as the name of the executable
        arg_token = strtok_r(cmd_token, " \t\n", &saveptr2);
        if (arg_token != NULL) {
            if (strlen(arg_token) >= EXE_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG; // check executable size and return if too long
            }
            strncpy(cmd->exe, arg_token, EXE_MAX - 1);
            cmd->exe[EXE_MAX - 1] = '\0';
        } else {
            continue;
        }

        // Tokenize remaining words as arguments
        char args_buffer[ARG_MAX] = "";
        int args_len = 0;
        while ((arg_token = strtok_r(NULL, " \t\n", &saveptr2)) != NULL) {
            int token_len = strlen(arg_token);
            if (args_len + token_len + 1 >= ARG_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG; // check argument size and return if too long
            }
            strcat(args_buffer, arg_token);
            strcat(args_buffer, " ");
            args_len += token_len + 1;
        }
        if (args_len > 0) {
            args_buffer[args_len - 1] = '\0'; // Remove trailing space
            strncpy(cmd->args, args_buffer, ARG_MAX - 1);
            cmd->args[ARG_MAX - 1] = '\0';
        }

        cmd_index++; // moving to next command
    }
    clist->num = cmd_index; // storing total number of commands that are parsed
    
    return OK; // success
}
