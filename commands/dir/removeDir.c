//
// Created by vondr on 02.10.2022.
//

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>


#include "removeDir.h"
#include "../../input/checkInput.h"
#include "../../utils/error.h"
#include "../../utils/messages.h"


void rmdir_command(char* dir)
{
    int result = is_empty(dir);

    if (result == SUCCESS)
    {
        if (rmdir(dir) != 0)
        {
            result = REMOVE_ERROR;
        }
    }

    print_error_message(result);
}

int is_empty(char* dir)
{
    int result, counter = 0;
    struct stat path_stat;
    struct dirent *de;  // Pointer for directory entry

    result = directoryExists(dir);
    if (result != EXISTS)
    {
        return result;
    }

    // opendir() returns a pointer of DIR type.
    DIR *dr = opendir(dir);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        return UNDEFINED_ERROR;
    }

    // prepare variable with path
    // read items in directory
    // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
    // for readdir()
    while ((de = readdir(dr)) != NULL)
    {
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
            continue;

        counter++;
        break;
    }

    closedir(dr);

    if (counter > 0)
    {
        return NOT_EMPTY;
    }
    else
    {
        return SUCCESS;
    }
}