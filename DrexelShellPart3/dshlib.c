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
                break;
            }
            token++;
        }

        if (*token && !in_quotes) {
            *token = '\0';
            token++;
        }

        cmd->argv[cmd->argc++] = strdup(arg_start);
        if (cmd->argc >= CMD_ARGV_MAX - 1) break;
    }

    cmd->argv[cmd->argc] = NULL;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    memset(clist, 0, sizeof(command_list_t));  

    char *tokens[CMD_MAX];  
    int cmd_count = 0;

    char *token = strtok(cmd_line, PIPE_STRING);
    while (token != NULL) {
        if (cmd_count >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;  
        }
        tokens[cmd_count++] = token;  
        token = strtok(NULL, PIPE_STRING);
    }

    for (int i = 0; i < cmd_count; i++) {
        clist->commands[i].argc = 0;
        clist->commands[i]._cmd_buffer = strdup(tokens[i]); // Store full command

        char *arg = strtok(clist->commands[i]._cmd_buffer, " ");
        while (arg != NULL) {
            clist->commands[i].argv[clist->commands[i].argc++] = arg;
            arg = strtok(NULL, " ");
        }
        clist->commands[i].argv[clist->commands[i].argc] = NULL; // Null-terminate argv
    }

    clist->num = cmd_count;  
    return (clist->num > 0) ? OK : WARN_NO_CMDS;
}

void exec_cd(char *path) {
	 if (!path) return;
	 if (chdir(path) != 0) {
		  perror("cd failed"); 	 
	 	  exit(1);
	 }
}
 /*
 */
int execute_pipeline(command_list_t *clist) {
    int num_cmds = clist->num;
    int pipes[CMD_MAX - 1][2];  
    pid_t pids[CMD_MAX];  

    //create each pipe iniitalized here
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed");
            return ERR_EXEC_CMD;
        }
    }

    for (int i = 0; i < num_cmds; i++) {
        pids[i] = fork();

        if (pids[i] == -1) {
            perror("fork failed");
            return ERR_EXEC_CMD;
        }

        if (pids[i] == 0) {  //Child process
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);  
            }
            if (i < num_cmds - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);  
            }

            for (int j = 0; j < num_cmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp failed");
            exit(127);
        }
    }

    for (int i = 0; i < num_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < num_cmds; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return OK;
}

int exec_local_cmd_loop() {
    char cmd_buff[SH_CMD_MAX];
    command_list_t cmd_list;

    while (1) {
        printf("%s", SH_PROMPT);

        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        cmd_buff[strcspn(cmd_buff, "\n")] = '\0'; 

        if (strlen(cmd_buff) == 0 || cmd_buff[0] == '|') {
            printf("error: empty pipeline\n");
            continue;
        }

        char *token = strtok(cmd_buff, "|");
        cmd_list.num = 0;

        while (token != NULL && cmd_list.num < CMD_MAX) {
            parse_command(token, &cmd_list.commands[cmd_list.num]);
            cmd_list.num++;
            token = strtok(NULL, "|");
        }

        if (strcmp(cmd_list.commands[0].argv[0], EXIT_CMD) == 0) {
            printf("exiting...\n");
            exit(0);
        } else if (strcmp(cmd_list.commands[0].argv[0], "cd") == 0) {
            if (cmd_list.commands[0].argc < 2) {
                printf("cd: missing argument\n");
            } else {
                exec_cd(cmd_list.commands[0].argv[1]);
            }
            continue;  
        }

        execute_pipeline(&cmd_list);
    }

    return OK;
}
