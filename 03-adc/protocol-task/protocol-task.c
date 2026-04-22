#include"protocol-task.h"
#include "stdio.h"
#include "string.h"

static api_t* api = {0};
static int commands_count = 0;

void protocol_task_init(api_t* d_api)
{
    if (!d_api) return;
    api = d_api;

    int i = 0;
    while(d_api[i].command_name != NULL)
    {
        i++;
    }

    commands_count = i;
}

void protocol_task_handle(char* command_string)
{
    if (!command_string)
    {
        return;
    }   


    const char* command_name = command_string;
    const char* command_args = NULL;
    char* space_symbol = strchr(command_string, ' ');

    if (space_symbol)
    {
        *space_symbol = '\0';
        command_args = space_symbol + 1;
    }
    else
    {
        command_args = "";
    }


    for (int i = 0; i < commands_count; i++)
    {
        if (strcmp(api[i].command_name, command_name) == 0)
        {
            printf("\nResieved command \"%s\" with args: {%s}\n", command_name, command_args);
            api[i].command_callback(command_args);
            return;
        }
    
    }
    printf("\nError: there is no such command \"%s\"\n", command_name);
    return;
}