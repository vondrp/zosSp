//
// Created by vondr on 02.10.2022.

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>


#include "makeDir.h"
#include "../../input/checkInput.h"
#include "../../utils/error.h"
#include "../../utils/messages.h"

void mkdir_command(char * dirName)
{
    int check;
    char *remainingPath = malloc(strlen(dirName));

    removePathLastPart(remainingPath, dirName);
    printf("full: %s, remain: %s\n",dirName, remainingPath);
    if (strcmp(remainingPath, dirName) != 0)
    {
        check = directoryExists(remainingPath);

        if (check == EXISTS)
        {
            check = make_directory(dirName);
        }
    }
    else //if remainingPath and dirName are the same -> path not specified (is taking base directory)
    {
        check = make_directory(dirName);
    }
    print_error_message(check);

    free(remainingPath);
}

int make_directory(char * dirName)
{
    int check;

    check = directoryExists(dirName);

    if (check != PATH_NOT_FOUND)
    {
        return check;
    }
    check = mkdir(dirName);

    // check if directory is created or not
    if (!check)
        return SUCCESS;
    else {
        return UNDEFINED_ERROR;
    }
}