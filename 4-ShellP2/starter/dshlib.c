#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

 // helper function which trims trailing and leading spaces
 void trim_spaces(char *str) {
    char *start = str;
    char *end;

    // trimming leading spaces
    while (*start == SPACE_CHAR) {
        start++;
    }

    // trimming trailing spaces
    end = start + strlen(start) - 1;
    while (end > start && *end == SPACE_CHAR) {
        end--;
    }
    *(end + 1) = '\0'; // null terminate string

    memmove(str, start, strlen(start) + 1); // shifting trimmed string back to original buffer
 }

// parses command line into cmd_buff_t and deals with potential quotations
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    char *token; // used to store start of each token
    int argc = 0;
    bool in_quotes = false;
    char *start = cmd_line; // used to traverse input

    trim_spaces(cmd_line);

    // parsing command line into tokens
    while (*start != '\0' && argc < CMD_ARGV_MAX - 1) {
        while (*start == SPACE_CHAR && !in_quotes) {
            start++;
        }

        if (*start == '"') {
            in_quotes = true;
            start++; // move past quote since we found it
            token = start; // marking position of start of quoted string

            while (*start != '\0' && *start != '"') { // finding closing quote
                start++;
            }

            if (*start == '"') {
                *start = '\0';
                in_quotes = false;
                start++;
            }

        } else { // handling regular tokens that aren't in quotes
            token = start;
            while (*start != '\0' && (*start != SPACE_CHAR || in_quotes)) {
                start++;
            }

            if (*start == SPACE_CHAR) {
                *start = '\0';
                start++;
            }
        }

        cmd_buff->argv[argc++] = token;
    }

    cmd_buff->argv[argc] = NULL;
    cmd_buff->argc = argc;

    return OK;
}

 // function to handle actual execution of built-in commands
Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc == 1) {
            return BI_EXECUTED; // cd does nothing with no arguments
        } else {
            // execute change of directory
            if (chdir(cmd->argv[1]) != 0) {
                fprintf(stderr, "Error executing built-in command\n");
                return ERR_CMD_ARGS_BAD;
            }
            return BI_EXECUTED;
        }
    } else if (strcmp(cmd->argv[0], "exit") == 0) {
        return BI_CMD_EXIT;
    }

    return BI_NOT_BI; // otherwise not built in
 }

 // matches input to built in commands
 Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    } else if (strcmp(input, "exit") == 0) {
        return BI_CMD_EXIT;
    }
    
    return BI_NOT_BI; // otherwise not built in
 }

 int exec_cmd(cmd_buff_t *cmd) {
    pid_t pid = fork();
    if (pid == 0) {
        // child process
        execvp(cmd->argv[0], cmd->argv);
        fprintf(stderr, CMD_ERR_EXECUTE);
        exit(ERR_EXEC_CMD);
    } else if (pid > 0) {
        // parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
    } else {
        // fork failed
        return ERR_EXEC_CMD;
    }

    return OK;
 }

int exec_local_cmd_loop()
{
    char *cmd_buff;
    int rc = 0;
    cmd_buff_t cmd;

    // allocate memory for cmd_buff
    cmd_buff = malloc(SH_CMD_MAX);
    if (cmd_buff == NULL) {
        return ERR_MEMORY;
    }

    // TODO IMPLEMENT MAIN LOOP
    while (1) {
        printf("%s", SH_PROMPT);

        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n"); // Handles EOF
            break;
        }

        //remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';

        if (strlen(cmd_buff) == 0) { // check for empty commands
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        // parse command line into cmd struct
        rc = build_cmd_buff(cmd_buff, &cmd);
        if (rc != OK) {
            fprintf(stderr, "Error parsing command line\n");
            continue;
        }

        // check for built in commands
        Built_In_Cmds bi_cmd = match_command(cmd.argv[0]);
        if (bi_cmd == BI_CMD_EXIT) {
            free(cmd_buff);
            return OK_EXIT;
        } else if (bi_cmd == BI_CMD_CD) {
            rc = exec_built_in_cmd(&cmd);
            if (rc != BI_EXECUTED) {
                continue;
            }
            if (rc == ERR_CMD_ARGS_BAD) {
                fprintf(stderr, "Error executing built-in command\n");
            }
            continue;
        }


        // execute external commands
        rc = exec_cmd(&cmd);
        if (rc != OK) {
            fprintf(stderr, CMD_ERR_EXECUTE);
        }

    }

    free(cmd_buff);
    return OK;
}
