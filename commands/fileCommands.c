//
// Created by vondr on 06.10.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>

#include "fileCommands.h"

#include "../output/error.h"
#include "../output/messages.h"

#include "../input/checkInput.h"
#include "../input/inputHandler.h"

void cp_command (char *source_file, char *target_file)
{
    if (strlen(source_file) == 0)
    {
        print_error_message(SOURCE_FILE_NOT_FOUND);
        return;
    }

    if (strlen(target_file) == 0)
    {
        print_error_message(TARGET_PATH_NOT_FOUND);
        return;
    }

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

int copy_file(char *source_file_path, char *target_path)
{
    process_path(source_file_path);
    process_path(target_path);

    struct directory_item source_file = {};

    if(directory_exists(source_file_path, root_item, &source_file) != EXISTS
    || source_file.isFile == false)
    {
        return SOURCE_FILE_NOT_FOUND;
    }

    struct directory_item target_dir = {};
    struct directory_item target_dir_dad = {};

    // if given path already exist -> we cannot create copy with same name
    if (directory_exists(target_path, root_item, &target_dir) == EXISTS)
    {
        return TARGET_PATH_NOT_FOUND;
    }

    // find place where target dad is
    char *path_target_dir = malloc(strlen(target_path) * sizeof(char));

    remove_path_last_part(path_target_dir, target_path);
    if (directory_exists(path_target_dir, root_item, &target_dir) != EXISTS || target_dir.isFile == true)
    {
        free(path_target_dir);
        return TARGET_PATH_NOT_FOUND;
    }

    char *path_target_dad = malloc(strlen(target_path) * sizeof(char));

    remove_path_last_part(path_target_dad, path_target_dir);
    // found target directory must be found + must not be file
    if (directory_exists(path_target_dad, root_item, &target_dir_dad) != EXISTS || target_dir_dad.isFile == true)
    {
        return TARGET_PATH_NOT_FOUND;
    }

    if (equals(target_dir_dad, target_dir))
    {
        copy_direct_item(root_item, &target_dir_dad);
    }

    struct directory_item copy = {};

    copy_direct_item(&source_file, &copy);

    strcpy(copy.name, get_filename(target_path));

    int32_t fat_needed;
    // how many fat indexes needed
    fat_needed = ceil((double)copy.size / (double)global_br->cluster_size);

    int i;
    int32_t indexes_new[fat_needed]; //indexes of used fat
    int32_t indexes_old[fat_needed]; //indexes of used fat

    indexes_old[0] = source_file.start_cluster;
    for (i = 1; i < fat_needed; i++)
    {
        indexes_old[i] = fat_table[indexes_old[i - 1]];
    }

    // find all necessary fat indexes
    int check = find_free_fat_indexes(fat_needed, indexes_new);
    if (check != SUCCESS)
    {
        free(path_target_dad);
        return check;
    }
    char *cluster = malloc(global_br->cluster_size);

    // copy cluster conent
    for(i = 0; i < fat_needed; i++)
    {
        fseek(filePtr, global_br->data_start_address + indexes_old[i] * global_br->cluster_size, SEEK_SET);
        fread(cluster, global_br->cluster_size, 1, filePtr);

        fseek(filePtr, global_br->data_start_address + indexes_new[i] * global_br->cluster_size, SEEK_SET);
        fwrite(cluster, global_br->cluster_size, 1, filePtr);
    }

    for (i = 0; i < fat_needed - 1; i++)
    {
        fat_table[indexes_new[i]] = indexes_new[i + 1];
    }
    fat_table[indexes_new[fat_needed - 1]] = FAT_FILE_END;
    rewrite_fat();

    free(cluster);
    copy.start_cluster = indexes_new[0];

    write_dir(&target_dir, &copy, &target_dir_dad);
    free(path_target_dad);
    return SUCCESS;
}

void mv_command (char *source_file, char *target_file)
{
    if (strlen(source_file) == 0)
    {
        print_error_message(SOURCE_FILE_NOT_FOUND);
        return;
    }

    if (strlen(target_file) == 0)
    {
        print_error_message(TARGET_PATH_NOT_FOUND);
        return;
    }

    int result;
    process_path(source_file);
    process_path(target_file);

    result = check_filename_input(source_file);

    if (result != SUCCESS)
    {
        print_error_message(SOURCE_FILE_NOT_FOUND);
        return;
    }

    result = check_filename_input(target_file);

    if (result != SUCCESS)
    {
        print_error_message(result);
        return;
    }

    struct directory_item source = {};
    result = directory_exists(source_file, root_item, &source);

    if (result != EXISTS || source.isFile == false)
    {
        print_error_message(SOURCE_FILE_NOT_FOUND);
        return;
    }

    // malloc size is the size as ADD of both paths - used for both, like than does not have to realloc
    char *temp = malloc((strlen(source_file) + strlen(target_file)) * sizeof(char));
    struct directory_item source_parent = {};
    struct directory_item source_grandparent = {};

    remove_path_last_part(temp, source_file);

    directory_exists(temp, root_item, &source_parent);

    // if source and his parent are same -> path was in form /name -> parent is root
    if(equals(source, source_parent) == true)
    {
        copy_direct_item(root_item, &source_parent);
        copy_direct_item(root_item, &source_grandparent);
    }
    else
    {
        remove_path_last_part(temp, temp);
        directory_exists(temp, root_item, &source_grandparent);
        // if source and his parent are same -> path was in form /name -> parent is root
        if(equals(source_parent, source_grandparent) == true)
        {
            copy_direct_item(root_item, &source_grandparent);
        }
    }
    struct directory_item target_dir = {};
    result = directory_exists(target_file, root_item, &target_dir);

    // in case target points to directory move source file to it
    if (result == EXISTS && target_dir.isFile == false)
    {
        remove_path_last_part(temp, target_file);

        struct directory_item target_grandparent = {};
        directory_exists(temp, root_item, &target_grandparent);

        if(equals(source_parent, source_grandparent) == true)
        {
            copy_direct_item(root_item, &target_grandparent);
        }

        remove_from_directory(&source_parent, &source, &source_grandparent);
        write_dir(&target_dir, &source, &target_grandparent);
        result = SUCCESS;
    }
    else if (result == EXISTS && target_dir.isFile == true) // FILE cannot be place to another file
    {
        result = TARGET_PATH_NOT_FOUND;
    }
    else if (result != EXISTS) // if given directory not exists -> maybe in path given new filename
    {
        char *newFileName = malloc(strlen(target_file) * sizeof(char));

        strcpy(newFileName, get_filename(target_file));
        if(strlen(newFileName) > FILENAME_MAX)
        {
            free(temp);
            free(newFileName);
            print_error_message(TOO_LONG_FILENAME);
            return;
        }

        remove_path_last_part(temp, target_file);

        if (strcmp(temp, target_file) == 0)
        {
            result = EXISTS;
            copy_direct_item(root_item, &target_dir);
        }
        else
        {
            result = directory_exists(temp, root_item, &target_dir);
        }

        if (result == EXISTS && target_dir.isFile == false)
        {
            if (equals(target_dir, source_parent) == true) // if the directories are the same -> same directory but rename
            {
                struct directory_item help = {};
                // check if exist dir/file with the same name
                if (directory_exists(newFileName, &source_parent, &help) == EXISTS)
                {
                    result == EXISTS;
                }
                else
                {
                    rename_dir(&source, &source_parent, newFileName);
                }
            }
            else
            {
                remove_path_last_part(temp, temp);
                struct directory_item target_grandparent = {};
                directory_exists(temp, root_item, &target_grandparent);

                if(equals(target_dir, target_grandparent) == true)
                {
                    copy_direct_item(root_item, &target_grandparent);
                }

                remove_from_directory(&source_parent, &source, &source_grandparent);
                strcpy(source.name, newFileName);
                write_dir(&target_dir, &source, &target_grandparent);

            }
            free(newFileName);
            result = SUCCESS;
        }
        else
        {
            result = TARGET_PATH_NOT_FOUND;
        }
    }

    print_error_message(result);
    free(temp);
}

void rm_command(char *filename)
{
    if (strlen(filename) == 0)
    {
        print_error_message(FILE_NOT_FOUND);
    }

    int result;

    result = check_filename_input(filename);

    if (result == SUCCESS)
    {
        result = remove_file(filename);
    }
    print_error_message(result);
}

int remove_file(char * file_path)
{
    process_path(file_path);
    struct directory_item to_remove = {};
    if (directory_exists(file_path, root_item, &to_remove) != EXISTS || to_remove.isFile == false)
    {
        return FILE_NOT_FOUND;
    }

    struct directory_item parent = {};
    struct directory_item grandparent = {};

    int result;

    char *parentPath = malloc (strlen(file_path) * sizeof(char));
    remove_path_last_part(parentPath, file_path);
    // get parent
    directory_exists(parentPath, root_item, &parent);

    char *grandpaPath = malloc (strlen(file_path) * sizeof(char));
    remove_path_last_part(grandpaPath, parentPath);

    // if condition true -> remove items from root -> gradnpa and parent root
    if(strcmp(parent.name, to_remove.name) == 0)
    {
        copy_direct_item(root_item, &parent);
        copy_direct_item(root_item, &grandparent);
    }
    else
    {
        // find grandpa
        directory_exists(grandpaPath, root_item, &grandparent);

        if(strcmp(grandparent.name, parent.name) == 0)
        {
            copy_direct_item(root_item, &grandparent);
        }
    }

    result = remove_dir_item(&parent, &to_remove, &grandparent);

    return result;
}

void cat_command(char* filename)
{
    process_path(filename);

    char *path_without_filename = malloc(strlen(filename) * sizeof(char));
    remove_path_last_part(path_without_filename, filename);

    struct directory_item fileDirectory = {};
    if (strcmp(path_without_filename, filename) == 0) // if same -> choose current directory
    {
        fileDirectory = *current_dir;
    }
    else
    {   // found directory where file is located
        if (directory_exists(path_without_filename, root_item, &fileDirectory) != EXISTS)
        {
            free(path_without_filename);
            print_error_message(SOURCE_FILE_NOT_FOUND);
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
        print_error_message(SOURCE_FILE_NOT_FOUND);
        return;
    }

    int32_t used_fat = ceil((double)file.size / (double)global_br->cluster_size);
    int32_t usedFat[used_fat];

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
    if(strlen(toPlace) == 0)
    {
        print_error_message( TARGET_PATH_NOT_FOUND);
        return;
    }

    if(strlen(outsideFile) == 0)
    {
        print_error_message( SOURCE_FILE_NOT_FOUND);
        return;
    }


    FILE *fptr;
    struct directory_item toPlaceDir = {};
    long size = 0;
    int32_t fat_needed = 0;
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

    if (strlen(fileName) > FILENAME_MAX_LENGTH)
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
        if (directory_exists(toPlace, root_item, &toPlaceDir) != EXISTS)
        {
            print_error_message(TARGET_PATH_NOT_FOUND);
            fclose(fptr);
            return;
        }
    }

    struct directory_item exists = {};
    // check if in directory does not exists directory/file with the same name
    if (directory_exists(fileName, &toPlaceDir, &exists) == EXISTS)
    {
        print_error_message(TARGET_PATH_NOT_FOUND);
        fclose(fptr);
        return;
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
    int32_t fat_index; // last finded fat index
    int32_t indexes[fat_needed]; //indexes of used fat

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
    char *grandpaPath = malloc(strlen(toPlace) * sizeof(char));
    struct directory_item grandpaDir = {};
    remove_path_last_part(grandpaPath, toPlace);
    // directory already exists - checked before, method used to get grandpa directory

    // can happen only if the grandpa path is supposed to be root
    if (strcmp(grandpaPath, toPlace) == 0)
    {
        strcpy(grandpaPath, root_item->name);
    }

    directory_exists(grandpaPath, root_item, &grandpaDir);

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

    int32_t size_of_rest = fileStruct.size - (fat_needed - 1) * global_br->cluster_size;
    fread(cluster, size_of_rest, 1, fptr);
    fwrite(cluster, size_of_rest, 1, filePtr);

    free(cluster);

    //rewrite fat - save fat table changes to a file
    rewrite_fat();

    int32_t last_cluster_index = last_cluster(&toPlaceDir);
    int32_t clusterStart = global_br->data_start_address
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

void info_command(char* filename)
{
    if(strlen(filename) == 0)
    {
        print_error_message(SOURCE_FILE_NOT_FOUND);
        return;
    }

    process_path(filename);

    struct directory_item file = {};
    int check;
    int32_t cluster;
    check = directory_exists(filename, root_item, &file);

    if (check == PATH_NOT_FOUND)
    {
        // directory exists return path not found - but in this method we expect error msg FILE NOT FOUND
        print_error_message(SOURCE_FILE_NOT_FOUND);
        return;
    }

    printf("name: %s\nisFile: %d\nsize: %d\n Cluster:\n", file.name, file.isFile, file.size);

    cluster = file.start_cluster;
    do
    {
        printf("%d ", cluster);
        cluster = fat_table[cluster];
    } while (cluster != FAT_FILE_END && cluster != FAT_UNUSED && cluster != FAT_BAD_CLUSTER);
    printf("\n");
}

void outcp_command(char* file, char* toPlace)
{
    process_path(file);

    struct directory_item file_struct = {};
    if (directory_exists(file, root_item, &file_struct) != EXISTS && file_struct.isFile == false)
    {
        print_error_message(SOURCE_FILE_NOT_FOUND);
        return;
    }

    /*
    struct stat sb;
    printf("To place vypadá: %s\n", toPlace);
    if (stat(toPlace, &sb) == 0 && sb.st_mode & S_IFDIR) {
        printf("YES\n");
    } else {
        printf("NO\n");
    }

    if (is_outside_directory(toPlace))
    {
        printf("druha metoda YES\n");
    }
    */
    char *target_filename = get_filename(file);

    repair_back_slashes(toPlace);
    strcat(toPlace, "/");
    strcat(toPlace, target_filename);

    FILE *outsideF = fopen(target_filename, "wb");

    if (outsideF == NULL)
    {
        print_error_message(TARGET_PATH_NOT_FOUND);
        return;
    }

    int32_t fat_index = file_struct.start_cluster;
    char *cluster = malloc(global_br->cluster_size);
    int32_t size = global_br->cluster_size;
    do
    {
        fseek(filePtr, global_br->data_start_address + fat_index * global_br->cluster_size, SEEK_SET);
        fread(cluster, global_br->cluster_size, 1, filePtr);
        fat_index = fat_table[fat_index];
        if (fat_index == FAT_FILE_END)
        {
            size = file_struct.size % global_br->cluster_size;
        }

        fwrite(cluster, size, 1, outsideF);

    } while (fat_index != FAT_FILE_END);

    free(cluster);
    fclose(outsideF);


    // if given place where to place is not directory - cannot place here
    /*if (is_outside_directory(toPlace))
    {
    }
    else
    {
        print_error_message(TARGET_PATH_NOT_FOUND);
    }*/
}

void defrag_command(char *filename)
{
    process_path(filename);

    struct directory_item file_struct = {};
    if (directory_exists(filename, root_item, &file_struct)  != EXISTS && file_struct.isFile == false)
    {
        print_error_message(SOURCE_FILE_NOT_FOUND);
        return;
    }

    int32_t fat_n = ceil((double)file_struct.size / (double)global_br->cluster_size);
    int32_t original_fat[fat_n];

    original_fat[0] = file_struct.start_cluster;

    bool already_in_line = true;
    // to finish
    int i;
    for(i = 1; i < fat_n; i++)
    {
        original_fat[i] = fat_table[original_fat[i-1]];

        if (original_fat[i] - original_fat[i-1] != 1)
        {
            already_in_line = false;
        }
    }

    if (already_in_line == true)
    {
        print_error_message(SUCCESS);
        return;
    }

    clear_from_fat(&file_struct);

    int32_t new_fat[fat_n];

    int result = find_neighboring_free_fat_indexes(fat_n, new_fat);

    // if not success - must return original fat content
    if (result != SUCCESS)
    {
        for(i = 0; i < fat_n - 1; i++)
        {
            fat_table[original_fat[i]] = original_fat[i+1];
        }

        fat_table[original_fat[fat_n - 1]] = FAT_FILE_END;

        print_error_message(result);
        return;
    }
    else
    {
        char *cluster = malloc(global_br->cluster_size);
        // write backwards for case if at the start some cluster same
        for (i = fat_n - 1; i >= 0; i--)
        {
            // read from original cluster
            fseek(filePtr, global_br->data_start_address + original_fat[i] * global_br->cluster_size, SEEK_SET);
            fread(cluster, global_br->cluster_size, 1, filePtr);

            // write data to found cluster
            fseek(filePtr, global_br->data_start_address + new_fat[i] * global_br->cluster_size, SEEK_SET);
            fwrite(cluster, global_br->cluster_size, 1, filePtr);
        }

        // change struct start cluster info + rewrite it in the file
        struct directory_item parent_dir = {};
        char *path_parent = malloc(strlen(filename) * sizeof(char));

        remove_path_last_part(path_parent, filename);

        directory_exists(path_parent, root_item, &parent_dir);

        // if source and his parent are same -> path was in form /name -> parent is root
        if(equals(file_struct, parent_dir) == true)
        {
            copy_direct_item(root_item, &parent_dir);
        }
        file_struct.start_cluster = new_fat[0];
        upgrade_dir_item(&file_struct, &parent_dir);

        // rewrite fat table
        for (i = 0; i < fat_n - 1; i++)
        {
            fat_table[new_fat[i]] = new_fat[i+1];
        }
        fat_table[new_fat[fat_n - 1]] = FAT_FILE_END;
        // changes to a file
        rewrite_fat();
        free(path_parent);
        free(cluster);

        print_error_message(SUCCESS);
    }
}