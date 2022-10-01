//
// Created by vondr on 30.09.2022.
//
#include <stdio.h>

#include "removeFile.h"
#include "../input/checkInput.h"

#include "../utils/error.h"
#include "../utils/messages.h"

void rm_command(char *filename)
{
    int result;

    result = checkFilenameInput(filename);

    if (result == SUCCESS)
    {
        result = remove_file(filename);
    }
    print_error_message(result);
}

int remove_file(char * filename)
{
    int result;
    FILE *file;

    file = fopen(filename, "r");
    if (file)
    {
        fclose(file);

        if (remove(filename) == 0)
        {
            result = SUCCESS;
        }
        else
        {
            result = REMOVE_ERROR;
        }
    }
    else
    {
        result = FILE_NOT_FOUND;
    }

    return result;
}
