//
// Created by vondr on 29.09.2022.
//
#include <stdio.h>
#include <string.h>

#include "copyFile.h"
#include "../../utils/error.h"
#include "../../utils/messages.h"

#include "../../input/checkInput.h"


void cp_command (char *source_file, char *target_file)
{
    int result;
    result = checkFilenameInput(source_file);

    if (result == SUCCESS)
    {
        result = checkFilenameInput(target_file);
    }

    if (result == SUCCESS) {
        result = copy_file(source_file, target_file);
    }

    print_error_message(result);
}

int copy_file (char *source_file, char *target_file)
{
    char ch;
    FILE *source, *target;

    source = fopen(source_file, "r");

    if( source == NULL )
    {
        return FILE_NOT_FOUND;
    }

    target = fopen(target_file, "w");

    if( target == NULL )
    {
        fclose(source);
        return PATH_NOT_FOUND;
    }

    while( ( ch = fgetc(source) ) != EOF )
    {
        fputc(ch, target);
    }

    fclose(source);
    fclose(target);
    return SUCCESS;
}
