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

    count = count + 2; // +1 before first / and +1 after last /
    int result = PATH_NOT_FOUND;

    char **path_parts = malloc((count) * sizeof(char *)); // array of words; +1 - for safety cases
    struct directory_item *controlled_dir = root_item;

    int path_parts_num = split_path(path, path_parts, count);
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

        if (is_in_dir(controlled_dir, path_parts[i], nextDir) == true && nextDir->isFile == false)
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

        int path_char = 0;
        for (i = 0; path[i] != 0; i++)
        {
            path_char = path_char + 1;
        }
        // check if current path does not have allocated not enough memory
        if (path_char >= curr_path_max_length)
        {
            curr_path_max_length = path_char * 2;
            curr_path = realloc(curr_path, curr_path_max_length * sizeof(char));
        }

        strcpy(curr_path, path);
        result = SUCCESS;
    }

    free(path_parts);
    print_error_message(result);
}