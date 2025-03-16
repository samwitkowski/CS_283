#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

void trim_spaces(char *str)
{
    char *start = str;
    while (*start == SPACE_CHAR) start++;  

    char *end = start + strlen(start) - 1;
    while (end > start && *end == SPACE_CHAR) *end-- = '\0';  

    memmove(str, start, end - start + 2);  
}

int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    memset(clist, 0, sizeof(command_list_t));  // 

    char *tokens[CMD_MAX];  //command strings array to follow
    int cmd_count = 0;

    char *token = strtok(cmd_line, PIPE_STRING);
    while (token != NULL)
    {
        if (cmd_count >= CMD_MAX)
        {
            return ERR_TOO_MANY_COMMANDS;  
        }

        trim_spaces(token);  //remove spaces before storing
        tokens[cmd_count++] = token;  //save command segment
        token = strtok(NULL, PIPE_STRING);
    }

    for (int i = 0; i < cmd_count; i++)
    {
        char *cmd = strtok(tokens[i], " ");  
        if (!cmd || strlen(cmd) >= EXE_MAX)
        {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        strncpy(clist->commands[i].exe, cmd, EXE_MAX - 1);

        char *args = strtok(NULL, "");  
        if (args && strlen(args) >= ARG_MAX)
        {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        if (args)
        {
            strncpy(clist->commands[i].args, args, ARG_MAX - 1);
        }
        else
        {
            clist->commands[i].args[0] = '\0';  
        }
    }

    clist->num = cmd_count;  
    return (clist->num > 0) ? OK : WARN_NO_CMDS;
}
