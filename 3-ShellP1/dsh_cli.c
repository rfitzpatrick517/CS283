    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include "dshlib.h"

    /*
    * Implement your main function by building a loop that prompts the
    * user for input.  Use the SH_PROMPT constant from dshlib.h and then
    * use fgets to accept user input.  Since we want fgets to also handle
    * end of file so we can run this headless for testing we need to check
    * the return code of fgets.  I have provided an example below of how
    * to do this assuming you are storing user input inside of the cmd_buff
    * variable.
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
    *
    *   Expected output:
    *
    *      CMD_OK_HEADER      if the command parses properly. You will
    *                         follow this by the command details
    *
    *      CMD_WARN_NO_CMD    if the user entered a blank command
    *      CMD_ERR_PIPE_LIMIT if the user entered too many commands using
    *                         the pipe feature, e.g., cmd1 | cmd2 | ... |
    *
    *  See the provided test cases for output expectations.
    */

    int main()
    {
        char *cmd_buff = malloc(SH_CMD_MAX);
        command_list_t clist;

        while (1) {
            printf("%s", SH_PROMPT);

            if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
                printf("\n"); // Handles EOF
                break;
            }

            //remove the trailing \n from cmd_buff
            cmd_buff[strcspn(cmd_buff,"\n")] = '\0';

            if (strcmp(cmd_buff, EXIT_CMD) == 0) { // checking for exit command
                break;
            }

            //checking for dragon command
            if (strcmp(cmd_buff, "dragon") == 0) {
                printf("Not yet implemented");
                continue;
            }

            //checking if command is empty
            if (strlen(cmd_buff) == 0) {
                printf(CMD_WARN_NO_CMD);
                continue;
            }

            int rc = build_cmd_list(cmd_buff, &clist); // parse command line using build_cmd_list

            // Handles errors after parsing command line
            if (rc == ERR_TOO_MANY_COMMANDS) { // too many commands entered
                printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
                continue;
            } else if (rc != OK) {
                fprintf(stderr, "Error parsing command line\n");
                continue;
            }

            printf(CMD_OK_HEADER, clist.num); // printing header
            // printing each command with its arguments
            for (int i = 0; i < clist.num; i++) {
                printf("<%d> %s", i + 1, clist.commands[i].exe);
                if (strlen(clist.commands[i].args) > 0) {
                    printf(" [%s]", clist.commands[i].args); // prints arguments in brackets
                }
                printf("\n");
            }
        }

        free(cmd_buff);
        return 0; // exits program
    }
