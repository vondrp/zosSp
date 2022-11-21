//
// Created by vondr on 06.10.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
    process_path(filename);

    char *path_without_filename = malloc(sizeof(filename));
    remove_path_last_part(path_without_filename, filename);

    struct directory_item fileDirectory = {};
    if (strcmp(path_without_filename, filename) == 0) // if same -> choose current directory
    {
        fileDirectory = *current_dir;
    }
    else
    {   // found directory where file is located
        if (directory_exists(path_without_filename, root_item, &fileDirectory) == false)
        {
            free(path_without_filename);
            print_error_message(FILE_NOT_FOUND);
            return;
        }
    }

    char *onlyFileName = get_filename(filename);

    if (onlyFileName[0] == '/')
    {
        onlyFileName = onlyFileName + 1;
    }

    struct directory_item file = {};

    // if not found in directory or is not file
    if (is_in_dir(&fileDirectory, onlyFileName, &file) == false || file.isFile == false)
    {
        free(path_without_filename);
        print_error_message(FILE_NOT_FOUND);
        return;
    }

    int used_fat = ceil((double)file.size / (double)global_br->cluster_size);
    int usedFat[used_fat];

    usedFat[0] = file.start_cluster;
    // to finish
    int i;
    for(i = 1; i < used_fat; i++)
    {
        usedFat[i] = fat_table[usedFat[i-1]];
    }

    char *cluster = malloc(global_br->cluster_size);
    for (i = 0; i < used_fat; i++)
    {
        fseek(filePtr, global_br->data_start_address + usedFat[i] * global_br->cluster_size, SEEK_SET);
        fread(cluster, global_br->cluster_size, 1, filePtr);
        printf("%s", cluster);
    }
    printf("\n");
    free(cluster);
    free(path_without_filename);
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


void incp_command(char* outsideFile, char* toPlace)
{
    FILE *fptr;
    struct directory_item toPlaceDir = {};
    long size = 0;
    int fat_needed = 0;
    // Open file
    // try open file - evaluate if alright
    fptr = fopen(outsideFile, "rb");
    if (fptr == NULL)
    {
        print_error_message(SOURCE_FILE_NOT_FOUND);
        return;
    }
    char *fileName;

    fileName = get_filename(outsideFile);

    if (strlen(fileName) > MAX_CHAR)
    {
        print_error_message(TOO_LONG_FILENAME);
        fclose(fptr);
        return;
    }

    if (strlen(toPlace) == 0)
    {
        toPlaceDir = *current_dir;
    }
    else
    {
        process_path(toPlace);
        // check if given directory exists
        if (directory_exists(toPlace, root_item, &toPlaceDir) == false)
        {
            print_error_message(TARGET_PATH_NOT_FOUND);
            fclose(fptr);
            return;
        }
    }

    // check if in directory enough space
    if (toPlaceDir.size + sizeof(struct directory_item) > global_br->cluster_size)
    {
        print_error_message(DIR_OUT_OF_SPACE);
        fclose(fptr);
        return;
    }

    // find out how many space I need
    fseek(fptr, 0, SEEK_END);
    size = ftell(fptr);
    rewind(fptr);


    // how many fat indexes needed
    fat_needed = ceil((double)size / (double)global_br->cluster_size);

    int i;
    int fat_index; // last finded fat index
    int indexes[fat_needed]; //indexes of used fat

    // find first needed fat
    fat_index = find_free_fat_index();
    indexes[0] = fat_index;
    fat_table[fat_index] = FAT_FILE_END; //temporary end of file
    // find rest of the fat
    for (i = 1; i < fat_needed; i++)
    {
        fat_index = find_free_fat_index();
        indexes[i] = fat_index;
        fat_table[indexes[i-1]] = fat_index;
        fat_table[fat_index] = FAT_FILE_END;
    }

    fat_table[indexes[fat_needed - 1]] = FAT_FILE_END;

    // not enough free space in fat
    if (fat_index == -1)
    {
        for (i = 0; i < fat_needed; i++)
        {
            fat_table[indexes[i]] = FAT_UNUSED;
        }
        print_error_message(OUT_OF_FAT);
        fclose(fptr);
        return;
    }

    // find grandparent of the new file directory item (parent of directory item where new file will be stored)
    char *grandpaPath = malloc(sizeof(toPlace));
    struct directory_item grandpaDir = {};
    remove_path_last_part(grandpaPath, toPlace);
    // directory already exists - checked before, method used to get grandpa directory
    directory_exists(toPlace, root_item, &grandpaDir);

    // init structure
    struct directory_item fileStruct = {};
    strcpy(fileStruct.name, fileName);
    fileStruct.size = (int)size;
    fileStruct.start_cluster = indexes[0];
    fileStruct.isFile = true;

    // write content of the file to the file
    char *cluster = malloc(global_br->cluster_size);
    for (i = 0; i < fat_needed - 1; i++)
    {
        fseek(filePtr, global_br->data_start_address + indexes[i] * global_br->cluster_size, SEEK_SET);
        fread(cluster, global_br->cluster_size, 1, fptr);
        fwrite(cluster, global_br->cluster_size, 1, filePtr);
    }
    // last part to read may be shorter
    fseek(filePtr, global_br->data_start_address + indexes[fat_needed - 1] * global_br->cluster_size, SEEK_SET);

    int size_of_rest = fileStruct.size - (fat_needed - 1) * global_br->cluster_size;
    fread(cluster, size_of_rest, 1, fptr);
    fwrite(cluster, size_of_rest, 1, filePtr);

    free(cluster);

    //rewrite fat
    rewrite_fat();

    int last_cluster_index = last_cluster(&toPlaceDir);
    long clusterStart = global_br->data_start_address
                        + last_cluster_index * global_br->cluster_size
                        + toPlaceDir.size;
    // zapsani noveho directory
    fseek(filePtr, clusterStart, SEEK_SET);
    fwrite(&fileStruct, sizeof (struct directory_item), 1, filePtr);

    // change parent size
    toPlaceDir.size = toPlaceDir.size + (int32_t)sizeof(struct directory_item);
    // upgrade parent
    upgrade_dir_item(&toPlaceDir, &grandpaDir);

    fclose(fptr);
    free(grandpaPath);
}