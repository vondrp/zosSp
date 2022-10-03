//
// Created by vondr on 02.10.2022.
//

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ls.h"
#include "../../utils/error.h"
#include "../../input/checkInput.h"
#include "../../utils/messages.h"
#include "../../input/inputHandler.h"

void ls_command(char* directory)
{
    char* dir;

    repairBackSlashes(directory);
    int result = 0;
    int len = strlen(directory); // length of the given directory


    if (len == 0)
    {
        dir = ".\0";
    }
    else
    {
        dir = directory;
        result = directoryExists(dir);
        if (result != EXISTS)
        {
            print_error_message(result);
        }

    }

    if (result == 0 || result == EXISTS)
    {
        write_out_dir(dir);
    }
}

int write_out_dir(char* dir)
{
    struct stat path_stat;
    char* inside_dir_item_path; // path to directory inside items
    char* dir_items_path; // path inside the given directory - dir\ /
    struct dirent *de;  // Pointer for directory entry

    // opendir() returns a pointer of DIR type.
    DIR *dr = opendir(dir);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        return UNDEFINED_ERROR;
    }

    // variable with path to inside the repository
    dir_items_path = malloc((strlen(dir) + 2 ) * sizeof (char));
    strcpy(dir_items_path, dir);
    strcat(dir_items_path, "/");

    // prepare variable with path
    // read items in directory
    // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
    // for readdir()
    while ((de = readdir(dr)) != NULL)
    {
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
            continue;

        inside_dir_item_path = malloc ((strlen(dir_items_path) + strlen(de->d_name)) * sizeof(char));
        strcpy(inside_dir_item_path, dir_items_path);
        //inside_dir_item_path = dir +"/"+de->d_name;
        strcat(inside_dir_item_path, de->d_name);
        if (stat(inside_dir_item_path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode))
        {
            // it is directory
            printf("DIR: %s\n", de->d_name);
        }
        else
        {
            printf("FILE: %s \n", de->d_name);
        }
        free(inside_dir_item_path);
    }

    free(dir_items_path);
    closedir(dr);

    return SUCCESS;
}