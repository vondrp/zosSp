//
// Created by vondr on 24.09.2022.
//
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

#include "checkInput.h"
#include "../utils/error.h"

#define FILENAME_MAX_LENGTH 12

char* getFilename(char *string_path) {
    repairBackSlashes(string_path);
    char *dot = strrchr(string_path, '/');

    if(!dot || dot == string_path) return string_path;
    return dot + 1;
}

void repairBackSlashes(char * string)
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

int checkFilenameInput(char *string){
    char *fileName;

    repairBackSlashes(string);

    fileName = getFilename(string);
    if (strlen(fileName) > FILENAME_MAX_LENGTH)
    {
        return TOO_LONG_FILENAME;
    }

    return SUCCESS;
}

int directoryExists(char *directory)
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

void removePathLastPart(char *remainingPath, char *fullPath)
{
    unsigned long int len;
    repairBackSlashes(fullPath);

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