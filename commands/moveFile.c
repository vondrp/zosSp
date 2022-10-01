//
// Created by vondr on 30.09.2022.
//
#include <stdlib.h>
#include <stdio.h>

#include "moveFile.h"

#include "copyFile.h"
#include "../utils/messages.h"
#include "removeFile.h"

#include "../utils/error.h"
#include "../input/checkInput.h"

void mv_command (char *source_file, char *target_file)
{
    int result;
    result = checkFilenameInput(source_file);

    if (result == SUCCESS)
    {
        result = checkFilenameInput(target_file);
    }

    if (result == SUCCESS) {
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