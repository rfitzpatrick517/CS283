#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

int free_cmd_list(command_list_t *cmd_list) {
    if (cmd_list == NULL) {
        return ERR_MEMORY;
    }

    // Free each cmd_buff_t in the command list
    for (int i = 0; i < cmd_list->num; i++) {
        cmd_buff_t *cmd_buff = &cmd_list->commands[i];

        // Free the dynamically allocated command buffer
        if (cmd_buff->_cmd_buffer != NULL) {
            free(cmd_buff->_cmd_buffer);
            cmd_buff->_cmd_buffer = NULL;
        }
    }

    cmd_list->num = 0;

    return OK;
}

/*
 * Helper function to trim leading and trailing spaces.
 */
void trim_spaces(char *str) {
    char *start = str;
    char *end;

    // Trim leading spaces
    while (*start == SPACE_CHAR) {
        start++;
    }

    // Trim trailing spaces
    end = start + strlen(start) - 1;
    while (end > start && *end == SPACE_CHAR) {
        end--;
    }
    *(end + 1) = '\0'; // Null-terminate the trimmed string

    memmove(str, start, strlen(start) + 1); // Shift the trimmed string back to the start of the buffer
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    char *token; // Used to store the start of each token
    int argc = 0;
    bool in_quotes = false;
    char *start = cmd_line; // Used to traverse the input

    // Allocate memory for _cmd_buffer
    cmd_buff->_cmd_buffer = strdup(cmd_line);
    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }

    start = cmd_buff->_cmd_buffer; // Use the copied buffer
    trim_spaces(start);

    // Initialize redirection fields
    cmd_buff->input_file = NULL;
    cmd_buff->output_file = NULL;

    // Parsing command line into tokens
    while (*start != '\0' && argc < CMD_ARGV_MAX - 1) {
        while (*start == SPACE_CHAR && !in_quotes) {
            start++;
        }

        if (*start == '"') {
            in_quotes = true;
            start++; // Move past the opening quote
            token = start; // Mark the start of the quoted string

            // Find the closing quote
            while (*start != '\0' && *start != '"') {
                start++;
            }

            if (*start == '"') {
                *start = '\0'; // Null-terminate the quoted string at the closing quote
                in_quotes = false;
                start++; // Move past the closing quote
            } else {
                fprintf(stderr, "Error: Unmatched quote\n");
                free(cmd_buff->_cmd_buffer);
                cmd_buff->_cmd_buffer = NULL;
                return ERR_CMD_ARGS_BAD;
            }
        } else { // Handling regular tokens that aren't in quotes
            token = start;
            while (*start != '\0' && (*start != SPACE_CHAR || in_quotes)) {
                start++;
            }

            if (*start == SPACE_CHAR) {
                *start = '\0'; // Null-terminate the token at the space
                start++;
            }
        }

        // Check for redirection symbols - extra credit
        if (strcmp(token, "<") == 0) {
            // Input redirection
            cmd_buff->input_file = start; // Store the filename
            while (*start != '\0' && *start != SPACE_CHAR) {
                start++;
            }
            *start = '\0'; // Null-terminate the filename
            start++;
            continue; // Skip adding "<" to argv
        } else if (strcmp(token, ">") == 0) {
            // Output redirection
            cmd_buff->output_file = start; // Store the filename
            while (*start != '\0' && *start != SPACE_CHAR) {
                start++;
            }
            *start = '\0'; // Null-terminate the filename
            start++;
            continue; // Skip adding ">" to argv
        }

        // Add the parsed token to the argv array
        cmd_buff->argv[argc++] = token;
    }

    cmd_buff->argv[argc] = NULL;
    cmd_buff->argc = argc;

    return OK;
}

/*
 * Builds a command list by splitting the input line by pipes.
 */
 int build_cmd_list(char *cmd_line, command_list_t *cmd_list) {
    char cmd_line_copy[SH_CMD_MAX]; // Create a copy of the input command line
    strncpy(cmd_line_copy, cmd_line, SH_CMD_MAX);
    cmd_line_copy[SH_CMD_MAX - 1] = '\0';

    char *token;
    int cmd_count = 0;

    trim_spaces(cmd_line_copy);

    // Split the input line by pipes
    token = strtok(cmd_line_copy, PIPE_STRING);
    while (token != NULL && cmd_count < CMD_MAX) {
        // Build a cmd_buff_t for each command
        int rc = build_cmd_buff(token, &cmd_list->commands[cmd_count]);
        if (rc != OK) {
            fprintf(stderr, "Error parsing command\n");
            return rc;
        }
        cmd_count++;
        token = strtok(NULL, PIPE_STRING);
    }

    cmd_list->num = cmd_count;
    return OK;
}

/*
 * Executes built-in commands (e.g., cd, exit).
 */
Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc == 1) {
            return BI_EXECUTED; // cd does nothing with no arguments
        } else {
            // Execute change of directory
            if (chdir(cmd->argv[1]) != 0) {
                perror("Error executing built-in command");
                return ERR_CMD_ARGS_BAD;
            }
            return BI_EXECUTED;
        }
    } else if (strcmp(cmd->argv[0], "exit") == 0) {
        return BI_CMD_EXIT;
    }

    return BI_NOT_BI; // Not a built-in command
}

/*
 * Matches the input command to a built-in command.
 */
Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    } else if (strcmp(input, "exit") == 0) {
        return BI_CMD_EXIT;
    }

    return BI_NOT_BI; // Not a built-in command
}

/*
 * Executes an external command using fork/execvp.
 */
 int exec_cmd(cmd_buff_t *cmd) {
    pid_t pid = fork(); // Fork a child process
    if (pid == 0) {

        // Handle input redirection
        if (cmd->input_file != NULL) {
            int fd_in = open(cmd->input_file, O_RDONLY);
            if (fd_in == -1) {
                perror("open (input)");
                exit(ERR_EXEC_CMD);
            }
            dup2(fd_in, STDIN_FILENO); // Redirect stdin
            close(fd_in); // Close the file descriptor after duplication
        }

        // Handle output redirection
        if (cmd->output_file != NULL) {
            int fd_out = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out == -1) {
                perror("open (output)");
                exit(ERR_EXEC_CMD);
            }
            dup2(fd_out, STDOUT_FILENO); // Redirect stdout
            close(fd_out); // Close the file descriptor after duplication
        }

        // Execute the command
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp"); // Print error message if execvp fails
        exit(ERR_EXEC_CMD); // Exit with an error code
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0); // Wait for the child process to complete
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status); // Return the child's exit status
        }
    } else {
        // Fork failed
        perror("fork");
        return ERR_EXEC_CMD;
    }

    return OK;
}

/*
 * Executes a pipeline of commands.
 */
 int execute_pipeline(command_list_t *cmd_list) {
    int pipe_fds[CMD_MAX][2]; // Array of pipes
    pid_t pids[CMD_MAX]; // Array of child PIDs

    // Create pipes and fork child processes
    for (int i = 0; i < cmd_list->num; i++) {
        if (i < cmd_list->num - 1) {
            // Create a pipe for each pair of adjacent commands
            if (pipe(pipe_fds[i]) == -1) {
                perror("pipe");
                return ERR_EXEC_CMD;
            }
        }

        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            if (i > 0) {
                // Redirect stdin from the previous pipe
                dup2(pipe_fds[i - 1][0], STDIN_FILENO);
                close(pipe_fds[i - 1][0]);
                close(pipe_fds[i - 1][1]);
            }

            if (i < cmd_list->num - 1) {
                // Redirect stdout to the next pipe
                dup2(pipe_fds[i][1], STDOUT_FILENO);
                close(pipe_fds[i][0]);
                close(pipe_fds[i][1]);
            }

            // Execute the command
            execvp(cmd_list->commands[i].argv[0], cmd_list->commands[i].argv);
            perror("execvp");
            exit(ERR_EXEC_CMD);
        } else if (pid > 0) {
            // Parent process
            pids[i] = pid;

            if (i > 0) {
                // Close the previous pipe
                close(pipe_fds[i - 1][0]);
                close(pipe_fds[i - 1][1]);
            }
        } else {
            // Fork failed
            perror("fork");
            return ERR_EXEC_CMD;
        }
    }

    // Close any remaining pipes in the parent process
    if (cmd_list->num > 1) {
        close(pipe_fds[cmd_list->num - 2][0]);
        close(pipe_fds[cmd_list->num - 2][1]);
    }

    // Wait for all child processes to complete
    for (int i = 0; i < cmd_list->num; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }

    return OK;
}



/*
 * Main loop of the shell.
 */
 int exec_local_cmd_loop() {
    char *cmd_buff;
    int rc = 0;

    // Allocate memory for the command buffer
    cmd_buff = malloc(SH_CMD_MAX);
    if (cmd_buff == NULL) {
        return ERR_MEMORY;
    }

    while (1) {
        printf("%s", SH_PROMPT); // Print the shell prompt

        // Read user input
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n"); // Handle EOF
            break;
        }

        // Remove the trailing newline character
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        // Skip empty commands
        if (strlen(cmd_buff) == 0) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        // Parse the command line into a command list
        command_list_t cmd_list;
        rc = build_cmd_list(cmd_buff, &cmd_list);
        if (rc != OK) {
            fprintf(stderr, "Error parsing command line\n");
            continue;
        }

        // Check for too many pipes
        if (cmd_list.num >= CMD_MAX) {
            fprintf(stderr, CMD_ERR_PIPE_LIMIT, CMD_MAX);
            free_cmd_list(&cmd_list);
            continue;
        }

        // Execute the commands
        if (cmd_list.num == 1) {
            // Single command
            Built_In_Cmds bi_cmd = match_command(cmd_list.commands[0].argv[0]);
            if (bi_cmd == BI_CMD_EXIT) {
                free_cmd_list(&cmd_list);
                free(cmd_buff);
                fprintf(stdout, "exiting...");
                return OK;
            } else if (bi_cmd == BI_CMD_CD) {
                rc = exec_built_in_cmd(&cmd_list.commands[0]);
                if (rc != BI_EXECUTED) {
                    fprintf(stderr, CMD_ERR_EXECUTE);
                }
                continue;
            }

            // External command
            rc = exec_cmd(&cmd_list.commands[0]);
            if (rc != OK) {
                fprintf(stderr, CMD_ERR_EXECUTE);
            }
        } else {
            // Pipeline of commands
            rc = execute_pipeline(&cmd_list);
            if (rc != OK) {
                fprintf(stderr, CMD_ERR_EXECUTE);
            }
        }

        // Free the command list
        free_cmd_list(&cmd_list);
    }

    free(cmd_buff);
    return OK;
}