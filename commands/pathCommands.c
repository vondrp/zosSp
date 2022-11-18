//
// Created by vondr on 03.10.2022.
//

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "../input/checkInput.h"
#include "../utils/error.h"
#include "../utils/messages.h"

void pwd_command()
{
    printf("\n");
    printf("%s\n", curr_path);
    printf("\n");
}

void cd_command(char *path)
{
    process_path(path);
    int i, count = 0;
    for (i = 0; i < strlen(path); i++)
    {
        if(path[i] == '/')
            count = count + 1;
    }

    int result = PATH_NOT_FOUND;

    char **path_parts = malloc((count + 3) * sizeof(char *)); // array of words; 3- bezpecnosti cislo
    struct directory_item *controlled_dir = root_item;

    int path_parts_num = split_path(path, path_parts);
    struct directory_item *nextDir = malloc(sizeof(struct directory_item));

    if (path_parts_num == 0) // POKUD zadne casi -> root
    {
        result = EXISTS;
    }

    for (i = 0; i < path_parts_num; i++)
    {
        if (strlen(path_parts[i]) == 0)
        {
            result = EXISTS;
            break;
        }

        if (is_in_dir(controlled_dir, path_parts[i], nextDir) == true )
        {
            controlled_dir = nextDir;
            result = EXISTS;
        }
        else
        {
            result = PATH_NOT_FOUND;
            break;
        }
    }

    if (result != PATH_NOT_FOUND)
    {
        current_dir = controlled_dir;
        printf("Curr dir %s\n", current_dir->name);
        strcpy(curr_path, path);
        result = SUCCESS;
    }

    free(path_parts);
    print_error_message(result);
}