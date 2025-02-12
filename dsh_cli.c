#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dshlib.h"

int main()
{
    char *cmd_buff = malloc(SH_CMD_MAX); 
    if (!cmd_buff)
    {
        perror("malloc failed");
        exit(1);
    }

    command_list_t clist;
    int rc;

    while (1)
    {
        // 
        printf("%s", SH_PROMPT);

        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL)
        {
            printf("\n"); 
            break;
        }

        
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

       
        if (cmd_buff[0] == '\0')
        {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        if (strcmp(cmd_buff, EXIT_CMD) == 0)
        {
            break; 
        }

        rc = build_cmd_list(cmd_buff, &clist);

        if (rc == OK)
        {
            printf(CMD_OK_HEADER, clist.num);
            for (int i = 0; i < clist.num; i++)
            {
                printf("<%d> %s", i + 1, clist.commands[i].exe);
                if (clist.commands[i].args[0] != '\0')
                {
                    printf(" [%s]", clist.commands[i].args);
                }
                printf("\n");
            }
        }
        else if (rc == ERR_TOO_MANY_COMMANDS)
        {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
        }
    }

    free(cmd_buff); 
    return 0;
}
