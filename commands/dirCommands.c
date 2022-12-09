//
// Created by vondr on 06.10.2022.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "dirCommands.h"
#include "../input/checkInput.h"
#include "../utils/error.h"
#include "../utils/messages.h"

#include "../fat/fat.h"

void mkdir_command(char *dirName)
{
    process_path(dirName);

    if (strlen(get_filename(dirName)) > MAX_CHAR)
    {
        print_error_message(TOO_LONG_FILENAME);
        return;
    }
    int check;
    char *remainingPath = malloc(strlen(dirName));
    char *path_grandparent = malloc(strlen(dirName)); // cesta ke struture, kde se nachazi direc item, kam se bude vkladat nova polozka
    remove_path_last_part(remainingPath, dirName); // odriznuti nove polozky, pro pridani z cesty

    char *firstSymbolCut; // = malloc(strlen(dirName)); // zde ulozena cesta pokud prvni symbol /

    // odriznuti top 2 polozek z cesty
    char *name;
    struct directory_item dir_to_place = {};
    struct directory_item grand_parent = {};

    if (strcmp(remainingPath, dirName) != 0)
    {
        remove_path_last_part(path_grandparent, remainingPath);

        if (strcmp(remainingPath, path_grandparent) == 0 && strcmp(remainingPath, root_item->name) != 0 && path_grandparent[0] == '/')
        {
            // grandpa is root
            strcpy(path_grandparent, root_item->name);
        }

        struct directory_item *look_from;
        if (remainingPath[0] == '/')
        {
            firstSymbolCut = malloc(sizeof(remainingPath));
            strcpy(firstSymbolCut, remainingPath+1);
            look_from = root_item;
            // odstraneni /
            //firstSymbolCut = remainingPath + 1;
            strcpy(remainingPath, firstSymbolCut);
            free(firstSymbolCut);
        }
        else
        {
            look_from  = current_dir;
        }

        check = directory_exists(remainingPath, look_from, &dir_to_place);

        if (check == EXISTS)
        {
            // vime, ze grandparent existuje, vyuzijeme metodu k jeho ziskani
            directory_exists(path_grandparent, look_from, &grand_parent);
            name = get_filename(dirName);
            check = make_directory(name, &dir_to_place, &grand_parent);
        }
        else if (check == PATH_NOT_FOUND)
        {
            check = ENTERED_PATH_NOT_FOUND;
        }
    }
    else //if remainingPath and dirName are the same -> path not specified (is taking currenth dir)
    {
        remove_path_last_part(path_grandparent, curr_path);

        // founded curr path and gradpa path are same + the path is not root
        if (strcmp(curr_path, path_grandparent) == 0 && strcmp(curr_path, root_item->name) != 0 && path_grandparent[0] == '/')
        {
            // grandpa is root
            strcpy(path_grandparent, root_item->name);
        }

        directory_exists(path_grandparent, root_item, &grand_parent);

        if (remainingPath[0] == '/')
        {
            firstSymbolCut = remainingPath + 1;
            check = make_directory(firstSymbolCut, root_item, &grand_parent);
        }
        else
        {
            check = make_directory(dirName, current_dir, &grand_parent);
        }
    }
    print_error_message(check);

    free(remainingPath);
}

int make_directory(char *dirName, struct directory_item *dir_where_create, struct directory_item *grandparent_dir)
{
    //TODO zprava dulezita pro kontrolu, pred odezvdanim smazat
    //printf("V make dir dirname: %s, where_created %s, grandparent: %s\n", dirName, dir_where_create->name, grandparent_dir->name);
    struct directory_item new_dir = {};
    int check;

    struct directory_item last_finded_part = {}; // not used here
    check = directory_exists(dirName, dir_where_create, &last_finded_part);
    if (check != PATH_NOT_FOUND)
    {
        return check;
    }

    new_dir.isFile = false;
    new_dir.size = 0;
    strcpy(new_dir.name, dirName);

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

    write_dir(dir_where_create, &new_dir, grandparent_dir);
    return SUCCESS;
}


void rmdir_command(char* dir)
{
    process_path(dir);
    int result = is_empty(dir);

    // cannot allow to root directory to be removed
    if (strcmp(dir, root_item->name) == 0)
    {
        result = NOT_EMPTY_DIR;
    }

    struct directory_item toBeDestroyed = {};
    struct directory_item parent = {};
    struct directory_item grand_parent = {};
    if (result == SUCCESS)
    {
        directory_exists(dir, root_item, &toBeDestroyed);

        char *parentPath = malloc (sizeof(dir));
        remove_path_last_part(parentPath, dir);
        // get parent
        directory_exists(parentPath, root_item, &parent);

        char *grandpaPath = malloc (sizeof(dir));
        remove_path_last_part(grandpaPath, parentPath);

        // if condition true -> remove items from root -> gradnpa and parent root
        if(equals(parent, toBeDestroyed) == true)
        {
            copy_direct_item(root_item, &parent);
            copy_direct_item(root_item, &grand_parent);
        }
        else
        {
            // find grandpa
            directory_exists(grandpaPath, root_item, &grand_parent);

            if(equals(grand_parent, parent) == true)
            {
                copy_direct_item(root_item, &grand_parent);
            }
        }

        remove_dir_item(&parent, &toBeDestroyed, &grand_parent);
    }

    print_error_message(result);
}

int is_empty(char *dir)
{
    int result;

    struct directory_item *source;
    if (dir[0] == '/')
    {
        source = root_item;
    }
    else
    {
        source = current_dir;
    }

    struct directory_item inPlace = {}; // ulozeni posledniho directory
    result = directory_exists(dir, source, &inPlace);
    if (result != EXISTS)
    {
        return result;
    }

    if (inPlace.size > 0)
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
    struct directory_item *look_from;
    if (strlen(directory) == 0)
    {
        look_from = current_dir;
    }
    else
    {
        look_from = root_item;
        process_path(directory);
    }
    //todo ODSTRANIT pred odezvdanim
    //printf("Look from: %s\n", look_from->name);
    print_error_message(write_out_dir(directory, look_from));
}

int write_out_dir(char* dir_path, struct directory_item *look_from)
{
    int result;
    struct directory_item directoryItem = {};
    int i, j;

    if (strlen(dir_path) == 0)
    {
        directoryItem = *look_from;
    }
    else
    {
        result = directory_exists(dir_path, look_from, &directoryItem);
        if (result != EXISTS)
        {
            return result;
        }
    }

    long clusterStart = global_br->data_start_address + directoryItem.start_cluster * global_br->cluster_size;
    fseek(filePtr, clusterStart, SEEK_SET);

    unsigned long howMany = directoryItem.size / sizeof(struct directory_item);

    struct directory_item directories[howMany];
    printf("\n");
    for (i = 0; i < howMany; i++)
    {
        fread(&directories[i], sizeof(struct directory_item), 1, filePtr);
        printf("%s\n", directories[i].name);
    }
    printf("\n");

    return SUCCESS;
}