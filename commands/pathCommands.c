//
// Created by vondr on 03.10.2022.
//
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <malloc.h>

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
    int result, i;
    char **path_parts = malloc(3 * sizeof(char *)); // array of words; 3- nahodne cislo pro zacatek
    struct directory_item last_dir = {};

    //TODO dodelat jen last_dir placeholder
    result = directory_exists(path, &last_dir, &last_dir);
    if (result != EXISTS)
    {
        print_error_message(result);
    }

    struct directory_item *controlled_dir;
    if (path[0] == '/')
    {
        controlled_dir = root_item;
    }
    else
    {
        controlled_dir = current_dir;
    }

    int path_parts_num = split_path(path, path_parts);

    strcpy(curr_path, controlled_dir->name);
    strcat(curr_path, "/");

    struct directory_item *nextDir = malloc(sizeof(struct directory_item));
    for (i = 0; i < path_parts_num; i++)
    {
        if (is_in_dir(controlled_dir, path_parts[i], nextDir) == true )
        {
            strcat(curr_path, "/");
            strcat(curr_path, nextDir->name);
            controlled_dir = nextDir;
        }
    }
    current_dir = controlled_dir;

    free(path_parts);
    print_error_message(result);
}