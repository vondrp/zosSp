//
// Created by vondr on 03.10.2022.
//
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

#include "../input/checkInput.h"
#include "../utils/error.h"
#include "../utils/messages.h"

void pwd_command()
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("PATH: %s\n", cwd);
    } else {
        perror("getcwd() error");
    }
}

void cd_command(char *path)
{
    int result;
    result = directory_exists(path);
    if (result != EXISTS)
    {
        print_error_message(result);
    }

    result = chdir(path);

    print_error_message(result);
}