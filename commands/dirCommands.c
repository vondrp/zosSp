//
// Created by vondr on 06.10.2022.
//

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include <string.h>

#include "dirCommands.h"
#include "../input/checkInput.h"
#include "../utils/error.h"
#include "../utils/messages.h"

#include "../fat/fat.h"

void mkdir_command(char * dirName)
{
    int check;
    char *remainingPath = malloc(strlen(dirName));

    remove_path_last_part(remainingPath, dirName);

    //todo inicializovat
    struct directory_item *drs;
    if (strcmp(remainingPath, dirName) != 0)
    {
        check = directory_exists(remainingPath);

        if (check == EXISTS)
        {
            //check = make_directory(dirName, drs);
        }
        else if (check == PATH_NOT_FOUND)
        {
            check = ENTERED_PATH_NOT_FOUND;
        }
    }
    else //if remainingPath and dirName are the same -> path not specified (is taking base directory)
    {
        //check = make_directory(dirName, drs);
    }
    print_error_message(check);

    free(remainingPath);
}

int make_directory(char * dirName, struct directory_item *drs)
{
    int check;

    drs->isFile = false;
    drs->size = 0;
    //drs->start_cluster =
    //TODO zkontrolovat zda adresar uz existuje
    /*check = directory_exists(dirName);
    if (check != PATH_NOT_FOUND)
    {
        return check;
    }*/
    //check = mkdir(dirName);
    //TODO predelat
    // check if directory is created or not
    /*if (!check)
        return SUCCESS;
    else {
        return UNDEFINED_ERROR;
    }*/
    return SUCCESS;
}


void rmdir_command(char* dir)
{
    int result = is_empty(dir);

    /*
    if (result == SUCCESS)
    {
        if (rmdir(dir) != 0)
        {
            result = REMOVE_ERROR;
        }
    }*/

    print_error_message(result);
}

int is_empty(char* dir)
{
    int result, counter = 0;
    struct stat path_stat;
    struct dirent *de;  // Pointer for directory entry

    result = directory_exists(dir);
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
        return NOT_EMPTY_DIR;
    }
    else
    {
        return SUCCESS;
    }
}

void ls_command(char* directory)
{
    char* dir;

    repair_back_slashes(directory);
    unsigned long long len = strlen(directory); // length of the given directory

    if (len == 0)
    {
        dir = ".\0";
    }
    else
    {
        dir = directory;
    }

    print_error_message(write_out_dir(dir));
}

int write_out_dir(char* dir)
{
    int result;
    struct stat path_stat;
    char* inside_dir_item_path; // path to directory inside items
    char* dir_items_path; // path inside the given directory - dir\ /
    struct dirent *de;  // Pointer for directory entry

    result = directory_exists(dir);
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