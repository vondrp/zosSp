//
// Created by vondr on 06.10.2022.
//

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include <string.h>


#include "dirCommands.h"
#include "../input/checkInput.h"
#include "../utils/error.h"
#include "../utils/messages.h"

#include "../fat/fat.h"

void mkdir_command(char *dirName)
{
    int check;
    char *remainingPath = malloc(strlen(dirName));

    remove_path_last_part(remainingPath, dirName);

    char *name;

    struct directory_item *dir_to_place = malloc(sizeof(struct directory_item));

    if (strcmp(remainingPath, dirName) != 0)
    {
        struct directory_item *look_from;
        if (remainingPath[0] == '/')
        {
            look_from = root_item;
        }
        else
        {
            look_from  = current_dir;
        }

        check = directory_exists(remainingPath, look_from, dir_to_place);

        printf("Dir to place: %s\n", dir_to_place->name);
        if (check == EXISTS)
        {
            name = get_filename(dirName);
            check = make_directory(name, dir_to_place);
        }
        else if (check == PATH_NOT_FOUND)
        {
            check = ENTERED_PATH_NOT_FOUND;
        }

    }
    else //if remainingPath and dirName are the same -> path not specified (is taking currenth dir)
    {
        check = make_directory(dirName, current_dir);
    }
    print_error_message(check);

    free(dir_to_place);
    free(remainingPath);
}

int make_directory(char *dirName, struct directory_item *dir_where_create)
{
    struct directory_item new_dir = {};
    int check;

    struct directory_item last_finded_part = {}; // not used here
    check = directory_exists(dirName, dir_where_create, &last_finded_part );
    if (check != PATH_NOT_FOUND)
    {
        return check;
    }
    printf("Pokracuji dal %s\n", dir_where_create->name);

    new_dir.isFile = false;
    new_dir.size = 0;
    strcpy(new_dir.name, dirName);

    printf("Po strcpy\n");
    int fat_index = find_free_fat_index();

    if (fat_index == -1)
    {
        return OUT_OF_FAT;
    }
    else
    {
        fat_table[fat_index] = FAT_FILE_END;
        new_dir.start_cluster = fat_index;
        rewrite_fat();
    }

    write_dir(dir_where_create, &new_dir);
    return SUCCESS;
}


void rmdir_command(char* dir)
{
    int result = is_empty(dir);

    /*
    if (result == SUCCESS)
    {
        if (rmdir(dir) != 0)
        {
            result = REMOVE_ERROR;
        }
    }*/

    print_error_message(result);
}

int is_empty(char* dir)
{
    int result, counter = 0;
    struct stat path_stat;
    struct dirent *de;  // Pointer for directory entry

    //TODO upravit inPlace jen placeholder
    struct directory_item inPlace = {}; // ulozeni posledniho directory
    result = directory_exists(dir, &inPlace, &inPlace);
    if (result != EXISTS)
    {
        return result;
    }

    // opendir() returns a pointer of DIR type.
    DIR *dr = opendir(dir);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        return UNDEFINED_ERROR;
    }

    // prepare variable with path
    // read items in directory
    // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
    // for readdir()
    while ((de = readdir(dr)) != NULL)
    {
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
            continue;

        counter++;
        break;
    }

    closedir(dr);

    if (counter > 0)
    {
        return NOT_EMPTY_DIR;
    }
    else
    {
        return SUCCESS;
    }

}

void ls_command(char* directory)
{
    char* dir;
    struct directory_item *look_from;
    repair_back_slashes(directory);
    unsigned long long len = strlen(directory); // length of the given directory

    if (len == 0)
    {
        dir = curr_path;
        look_from = current_dir;
    }
    else
    {
        dir = directory;
        if (directory[0] == '/')
            look_from = root_item;
        else
            look_from = current_dir;
    }

    print_error_message(write_out_dir(dir, look_from));
}

int write_out_dir(char* dir_path, struct directory_item *look_from)
{
    int result;
    struct directory_item directoryItem = {};
    int i, j;
    //TODO dodela directoItem placeholder

    printf("Dir_path: %s look_from %s \n", dir_path, look_from->name);
    result = directory_exists(dir_path, look_from, &directoryItem);
    if (result != EXISTS)
    {
        return result;
    }

    printf("Directory item %s %d %d\n", directoryItem.name, directoryItem.start_cluster, directoryItem.size);
    long clusterStart = global_br->data_start_address + directoryItem.start_cluster * global_br->cluster_size;
    fseek(filePtr, clusterStart, SEEK_SET);

    unsigned long howMany = directoryItem.size / sizeof(struct directory_item);

    printf("How many %lu\n", howMany);
    struct directory_item directories[howMany];
    for (i = 0; i < howMany; i++)
    {
        //TODO pomoci fseek zmenit odkud ctu kdyz ke konci clusteru
        fread(&directories[i], sizeof(struct directory_item), 1, filePtr);
        printf("%s\n", directories[i].name);
    }


    return SUCCESS;
}