#ifndef _SHELL_H_
#define _SHELL_H_

#include "stddef.h"

typedef struct ShellCommand
{
    const char *command;
    int (*handler)(int, char *[]);
} ShellCommand;

typedef struct ShellImpl
{
    int (*send_char)(const char);
    char *rx_buffer;
    const size_t rx_buffer_size;
} ShellImpl;

void shell_init(const ShellImpl *impl);

void shell_set_commands(const ShellCommand *commands, const int commands_count);

void shell_set_unknown_command_handler(int (*handler)(int, char *[]));

void shell_recv_char(const char c);

void shell_process(void);

#endif