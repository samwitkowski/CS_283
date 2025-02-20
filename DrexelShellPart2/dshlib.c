#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"
//#define CMD_ARGV_MAX 16

void parse_command(char *input, cmd_buff_t *cmd) {
    if (!input || !cmd) return;

    cmd->argc = 0;
    cmd->_cmd_buffer = strdup(input);  

    char *token = cmd->_cmd_buffer;
    int in_quotes = 0;
    char *arg_start = NULL;

    while (*token) {
        while (*token == ' ' && !in_quotes) token++;

        if (*token == '\0') break;  

        if (*token == '"') {  
            in_quotes = 1;  
            token++;  
        }

        arg_start = token;

        while (*token && (in_quotes || *token != ' ')) {
            if (*token == '"') {
                in_quotes = 0;  
                *token = '\0';  
                token++;
                break;  // Stop processing this argument
            }
            token++;
        }

        // Null-terminate the argument at the first space
        if (*token && !in_quotes) {
            *token = '\0';
            token++;
        }

        cmd->argv[cmd->argc++] = arg_start;  // Store argument

        if (cmd->argc >= CMD_ARGV_MAX - 1) break;  
    }

    cmd->argv[cmd->argc] = NULL;  
}

void exec_cd(char *path) {
	 if (!path) return;
	 if (chdir(path) != 0) {
		  perror("cd failed"); 	 
	 	  exit(1);
	 }
}
 /*
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
 */
int exec_local_cmd_loop()
{
    char cmd_buff[SH_CMD_MAX];
    //int rc = 0;
    cmd_buff_t cmd;
    while (1) {
        printf("%s", SH_PROMPT);

        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';  // Remove newline

        if (strlen(cmd_buff) == 0) {
            printf("%s\n", CMD_WARN_NO_CMD);
            continue;
        }

        parse_command(cmd_buff, &cmd);

        if (strcmp(cmd.argv[0], EXIT_CMD) == 0) {
            break;
        }
        else if (strcmp(cmd.argv[0], "cd") == 0) {
            if (cmd.argc > 2) {
                fprintf(stderr, "cd: Too many arguments\n");
            } else {
                exec_cd(cmd.argv[1]);
            }
        }
        else {
            pid_t pid = fork();
            if (pid == 0) {  // Child process
                execvp(cmd.argv[0], cmd.argv);
                perror("Execution failed");
                exit(127);
            } else if (pid > 0) {  // Parent process
                int status;
                waitpid(pid, &status, 0);
            } else {
                perror("fork failed");
            }
        }

        free(cmd._cmd_buffer);  
    }

    return OK;

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"

}
