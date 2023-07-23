#include <stdio.h>
#include "shell.h"

int shell_handler_test(int argc, char *argv[])
{
    printf("Received '%s' command", argv[0]);

    for (int i = 1; i < argc; i++)
    {
        printf("\nArg #%d = %s", i, argv[i]);
    }

    printf("\n");
    return 0;
}

int shell_handler_unknown_command(int argc, char *argv[])
{
    printf("Received unknown command: %s\n", argv[0]);
    return 0;
}

#define RX_BUFFER_SIZE 128

int main(void)
{
    char rx_buffer[RX_BUFFER_SIZE] = {0};

    ShellImpl impl =
        {
            .send_char = putchar,
            .rx_buffer = rx_buffer,
            .rx_buffer_size = RX_BUFFER_SIZE,
        };

    ShellCommand commands[] =
        {
            {"test1", shell_handler_test},
            {"test2", shell_handler_test},
            {"test3", shell_handler_test},
        };

    shell_init(&impl);
    shell_set_commands(commands, sizeof(commands) / sizeof(ShellCommand));
    shell_set_unknown_command_handler(shell_handler_unknown_command);

    while (1)
    {
        char c = getchar();
        shell_recv_char(c);
        shell_process();
    }

    return 0;
}
