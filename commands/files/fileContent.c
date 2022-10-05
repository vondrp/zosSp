//
// Created by vondr on 05.10.2022.
//
#include <stdio.h>

#include "fileContent.h"
#include "../../utils/error.h"
#include "../../utils/messages.h"

void cat_command(char* filename)
{
    FILE *fptr;
    char c;

    // Open file
    fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        print_error_message(FILE_NOT_FOUND);
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