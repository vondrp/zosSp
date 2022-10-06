//
// Created by vondr on 06.10.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileCommands.h"

#include "../utils/error.h"
#include "../utils/messages.h"

#include "../input/checkInput.h"


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

void mv_command (char *source_file, char *target_file)
{
    int result;
    char* target_filename;
    result = checkFilenameInput(source_file);

    if (result == SUCCESS)
    {
        if (is_directory(target_file))
        {
            target_filename = getFilename(source_file);

            if (strcmp(target_filename, "") == 0)
            {
                target_filename = source_file;
            }

            realloc(target_file, (strlen(target_file) + strlen(target_file) + 2) * sizeof (char));

            strcat(target_file, "/");
            strcat(target_file, target_filename);
        }
        result = checkFilenameInput(target_file);
    }

    if (result == SUCCESS)
    {
        result = move_file(source_file, target_file);
    }

    print_error_message(result);
}

int move_file (char *source_file, char *target_file) {
    int result;

    result = copy_file(source_file, target_file);
    if (result != SUCCESS)
    {
        return result;
    }

    result = remove_file(source_file);

    return result;
}

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

void cat_command(char* filename)
{
    FILE *fptr;
    char c;

    // Open file
    fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        print_error_message(SOURCE_FILE_NOT_FOUND);
        return;
    }

    // Read contents from file
    c = fgetc(fptr);
    while (c != EOF)
    {
        printf ("%c", c);
        c = fgetc(fptr);
    }

    fclose(fptr);
    printf("\n");
}