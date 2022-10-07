//
// Created by vondr on 24.09.2022.
//
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#include "checkInput.h"
#include "../utils/error.h"

#define FILENAME_MAX_LENGTH 12

char* get_filename(char *string_path) {
    repair_back_slashes(string_path);
    char *dot = strrchr(string_path, '/');

    if(!dot || dot == string_path) return string_path;
    return dot + 1;
}

void repair_back_slashes(char * string)
{
    int index = 0;
    char c = (string)[index];
    // reparing backslahes
    while(c != '\0'){
        if(c == '\\'){
            (string)[index] = '/';
        }
        index++;
        c = (string)[index];
    }
}

int check_filename_input(char *string){
    char *fileName;

    repair_back_slashes(string);

    fileName = get_filename(string);
    if (strlen(fileName) > FILENAME_MAX_LENGTH)
    {
        return TOO_LONG_FILENAME;
    }

    return SUCCESS;
}

int directory_exists(char *directory)
{
    DIR* dir = opendir(directory);
    if (dir) {
        /* Directory exists. */
        closedir(dir);
        return EXISTS;
    } else if (ENOENT == errno) {
        /* Directory does not exist. */
        return PATH_NOT_FOUND;
    } else {
        /* opendir() failed for some other reason. */
        return UNDEFINED_ERROR;
    }
}

void remove_path_last_part(char *remainingPath, char *fullPath)
{
    unsigned long int len;
    repair_back_slashes(fullPath);

    for (len = strlen(fullPath); len > 0; len --)
    {
        if(fullPath[len] == '/'){
            break;
        }
    }

    if (len == 0)
    {
        strcpy(remainingPath, fullPath);
        return;
    }

    strncpy(remainingPath, fullPath, len);
    remainingPath[len] =  *"\0";
}

#include <sys/stat.h>

int is_directory(char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}