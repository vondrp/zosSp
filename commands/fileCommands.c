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
#include "../input/inputHandler.h"


void cp_command (char *source_file, char *target_file)
{
    int result;
    result = check_filename_input(source_file);

    if (result == SUCCESS)
    {
        result = check_filename_input(target_file);
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
    result = check_filename_input(source_file);

    if (result == SUCCESS)
    {
        if (is_directory(target_file))
        {
            target_filename = get_filename(source_file);

            if (strcmp(target_filename, "") == 0)
            {
                target_filename = source_file;
            }

            if(realloc(target_file, (strlen(target_file) + strlen(target_file) + 2) * sizeof (char)) == NULL)
            {
                result = MEMORY_PROBLEMS;
            }
            else
            {
                strcat(target_file, "/");
                strcat(target_file, target_filename);
            }
        }
        result = check_filename_input(target_file);
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

    result = check_filename_input(filename);

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
    c = (char) fgetc(fptr);
    while (c != EOF)
    {
        printf ("%c", c);
        c = (char) fgetc(fptr);
    }

    fclose(fptr);
    printf("\n");
}

void load_command(char* scriptFile)
{
    FILE *fptr;
    char *wPathScriptName; // script file name without path
    char *file_line; // line from a file
    char **words;       // file line separated into words
    int last_words_amount = 0; int words_amount = DEFAULT_WORDS_AMOUNT;
    int lenmax = INPUT_LENGTH * sizeof (char), len = lenmax;
    // Open file
    fptr = fopen(scriptFile, "r");
    if (fptr == NULL)
    {
        print_error_message(SOURCE_FILE_NOT_FOUND);
        return;
    }

    wPathScriptName = get_filename(scriptFile);

    words = malloc(words_amount * sizeof(char *)); // array of words
    // Read contents from file

    file_line = get_line(fptr);
    while (file_line != NULL)
    {
        // if line contains load command + name of the current scriptFile -> line will not be done
        if (strstr(file_line, "load") != NULL && strstr(file_line, wPathScriptName) != NULL)
        {
            file_line = get_line(fptr);
            continue;
        }
        // split words in line to array of words
        sentence_to_words(file_line, words, &last_words_amount);
        call_commands(words);    // call commands
        file_line = get_line(fptr);
    }
}