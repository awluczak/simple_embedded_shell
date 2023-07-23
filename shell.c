#include "shell.h"
#include "stdbool.h"
#include "string.h"

#define SHELL_PROMPT ("shell> ")
#define SHELL_MAX_ARGS (16)

static struct ShellContext
{
    int (*send_char)(const char);
    char *rx_buffer;
    size_t rx_buffer_size;
    size_t rx_size;
    ShellCommand *commands;
    int commands_count;
    int (*unknown_command_handler)(int, char *[]);
} shell = {0};

static bool is_initialized(void)
{
    return (shell.send_char != NULL &&
            shell.rx_buffer != NULL &&
            shell.rx_buffer_size != 0);
}

static bool is_rx_buffer_full(void)
{
    return (shell.rx_size >= shell.rx_buffer_size);
}

static void reset_rx_buffer(void)
{
    memset(shell.rx_buffer, 0, shell.rx_buffer_size);
    shell.rx_size = 0;
}

static void send_string(const char *str)
{
    for (const char *c = str; *c != '\0'; c++)
    {
        shell.send_char(*c);
    }
}

#define send_prompt() send_string(SHELL_PROMPT)

static ShellCommand *find_command(const char *command)
{
    for (int i = 0; i < shell.commands_count; i++)
    {
        if (!strcmp(command, shell.commands[i].command))
        {
            return &shell.commands[i];
        }
    }

    return NULL;
}

void shell_init(const ShellImpl *impl)
{
    shell.send_char = impl->send_char;
    shell.rx_buffer = impl->rx_buffer;
    shell.rx_buffer_size = impl->rx_buffer_size;
    shell.rx_size = 0;

    if (is_initialized())
    {
        send_prompt();
    }
}

void shell_set_commands(const ShellCommand *commands, const int commands_count)
{
    shell.commands = commands;
    shell.commands_count = commands_count;
}

void shell_set_unknown_command_handler(int (*handler)(int, char *[]))
{
    shell.unknown_command_handler = handler;
}

void shell_recv_char(const char c)
{
    if (!is_initialized() || is_rx_buffer_full())
    {
        return;
    }

    shell.rx_buffer[shell.rx_size++] = c;
}

void shell_process(void)
{
    if (!is_initialized())
    {
        return;
    }

    if (shell.rx_size > 0 && shell.rx_buffer[shell.rx_size - 1] == '\n')
    {
        char *argv[SHELL_MAX_ARGS] = {NULL};
        int argc = 0;
        bool next_arg = true;

        for (size_t i = 0; i < shell.rx_size && argc < SHELL_MAX_ARGS; i++)
        {
            char *c = &shell.rx_buffer[i];
            if (*c == ' ')
            {
                *c = '\0';
                next_arg = true;
            }
            else if (next_arg)
            {
                argv[argc++] = c;
                next_arg = false;
            }
        }

        shell.rx_buffer[shell.rx_size - 1] = '\0';

        if (argc > 0)
        {
            const ShellCommand *command = find_command(argv[0]);
            if (command && command->handler)
            {
                command->handler(argc, argv);
            }
            else if (shell.unknown_command_handler)
            {
                shell.unknown_command_handler(argc, argv);
            }
        }

        reset_rx_buffer();
        send_prompt();
    }
    else if (is_rx_buffer_full())
    {
        reset_rx_buffer();
    }
}