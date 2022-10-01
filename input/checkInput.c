//
// Created by vondr on 24.09.2022.
//
#include <string.h>

#include "checkInput.h"
#include "../utils/error.h"

#define FILENAME_MAX_LENGTH 12

char *getFilename(char *string_path) {
    char *dot = strrchr(string_path, '/');
    if(!dot || dot == string_path) return "";
    return dot + 1;
}

int checkFilenameInput(char *string){
    int index = 0;
    char c = (string)[index];
    char *fileName;

    // reparing backslahes
    while(c != '\0'){
        if(c == '\\'){
            (string)[index] = '/';
        }
        index++;
        c = (string)[index];
    }

    fileName = getFilename(string);

    if (strlen(fileName) > FILENAME_MAX_LENGTH)
    {
        return TOO_LONG_FILENAME;
    }

    return SUCCESS;
}