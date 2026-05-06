#include "stdio-task.h"
#include "stdio.h"
#include "pico/stdlib.h"
#include "string.h"

#define LINE_BUFFER_LEN 128

char line[LINE_BUFFER_LEN] = {0};

int line_idx;

void stdio_task_init()
{
    line_idx = 0;
}


char* stdio_task_handle()
{
    int symbol = getchar_timeout_us(0);
    if (symbol == PICO_ERROR_TIMEOUT)
    {
        return NULL;
    }

    if (symbol == '\b' || symbol == 127)
    {
        if (line_idx > 0)
        {
            putchar('\b');
            putchar(' ');
            putchar('\b');
            line[line_idx] = 0;
            line_idx--;
        }
        return NULL;
    }

    putchar(symbol);

    if (symbol == '\r' || symbol == '\n')
    {
        line[line_idx] = '\0';
        line_idx = 0;
        return line;
    }

    if (line_idx >= LINE_BUFFER_LEN)
    {
        line_idx = 0;
        return NULL;
    }

    line[line_idx] = symbol;
    line_idx++;

    return NULL;
}