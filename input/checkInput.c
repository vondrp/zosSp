//
// Created by vondr on 24.09.2022.
//
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>

#include "checkInput.h"
#include "../utils/error.h"
#include "../fat/fat.h"

#define FILENAME_MAX_LENGTH 12

#define DEFAULT_PARTS_AMOUNT 3

char* get_filename(char *string_path) {
    repair_back_slashes(string_path);
    char *dot = strrchr(string_path, '/');

    if(!dot || dot == string_path) return string_path;
    return dot + 1;
}

void repair_back_slashes(char * string)
{
    int index = 0;
    char c = (string)[index];
    // reparing backslahes
    while(c != '\0'){
        if(c == '\\'){
            (string)[index] = '/';
        }
        index++;
        c = (string)[index];
    }
}

int check_filename_input(char *string){
    char *fileName;

    repair_back_slashes(string);

    fileName = get_filename(string);
    if (strlen(fileName) > FILENAME_MAX_LENGTH)
    {
        return TOO_LONG_FILENAME;
    }

    return SUCCESS;
}

int directory_exists(char *dir_path, struct directory_item *look_from, struct directory_item *last_part)
{
   // path separated into parts
    char **path_parts = malloc(DEFAULT_PARTS_AMOUNT * sizeof(char *)); // array of words

    // existuje jde o root
    if (strcmp(dir_path, "/") == 0)
    {
        copy_direct_item(root_item, last_part);
        return EXISTS;
    }

    // DEFAULTNI HODNOTA -
    int result = PATH_NOT_FOUND;
    int path_parts_num = split_path(dir_path, path_parts);

    struct directory_item *founded_dir = malloc(sizeof (struct directory_item));
    struct directory_item tested[path_parts_num + 1];
    tested[0] = *look_from;
    int k;
    for (k = 0; k < path_parts_num; k++)
    {
        if (is_in_dir(&tested[k], path_parts[k], founded_dir) == true) {
            copy_direct_item(founded_dir, &tested[k+1]);
            result = EXISTS;
        } else {
            // directory not exits
            tested[path_parts_num] = tested[k];
            result = PATH_NOT_FOUND;
            break;
        }
    }

    copy_direct_item(&tested[path_parts_num], last_part);
    free(founded_dir);
    return result;
}

void remove_path_last_part(char *remainingPath, char *fullPath)
{
    unsigned long int len;
    repair_back_slashes(fullPath);

    for (len = strlen(fullPath); len > 0; len --)
    {
        if(fullPath[len] == '/'){
            break;
        }
    }

    if (len == 0)
    {
        strcpy(remainingPath, fullPath);
        return;
    }

    strncpy(remainingPath, fullPath, len);
    remainingPath[len] =  *"\0";
}

int is_directory(char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}


int split_path(char *path, char** path_parts)
{
    int path_amount = DEFAULT_PARTS_AMOUNT;
    int i,j,ctr, len_max;
    j = 0;
    ctr = 0;
    len_max = FILENAME_MAX_LENGTH;
    for (i = 0; i <= (strlen(path)); i++) {

        if (j == 0)
        {
            len_max = FILENAME_MAX_LENGTH;
            path_parts[ctr] = malloc(len_max * sizeof(char *));
        }
        else if (j >= len_max)
        {
            len_max = len_max * 2;
            path_parts[ctr] = realloc(path_parts[ctr], len_max * sizeof (char *));
        }

        // if space or NULL found, assign NULL into newString[ctr]
        //printf("Pozice %d, znak %c, isspace %d\n", i, console_input[i], isspace(console_input[i]));
        if (path[i] == '/') {

            // to ignore multiple white spaces
            if (j != 0)
            {
                path_parts[ctr][j] = '\0';
                ctr++;  //for next word
            }

            // realloc more space for paths if necessary
            if (ctr >= path_amount)
            {
                printf("Jsem v realloc v split\n");
                path_amount = path_amount * 2;
                unsigned long currSize = 0;
                for (j = 0; j < ctr; j++)
                    currSize = currSize + strlen(path_parts[j]);

                //TODO je treba reallocovat jinak
                path_parts = realloc(path_parts, path_amount * currSize);
            }
            j = 0;    //for next word, init index to 0
        } else {
            path_parts[ctr][j] = path[i];
            j++;
        }
    }
    if (strlen(path_parts[ctr]) > 0)
    {
        return ctr+1;// +1 - pri indexu 0 je velikost 1
    }

    return ctr;
}

bool is_in_dir(struct directory_item *directory_parent, char dir_name[13], struct  directory_item *found)
{
    int j;
    // rodic nesmi byt soubor
    if (directory_parent->isFile == true)
    {
        return false;
    }

    long clusterStart = global_br->data_start_address + directory_parent->start_cluster * global_br->cluster_size;
    fseek(filePtr, clusterStart, SEEK_SET);

    unsigned long howMany = directory_parent->size / sizeof(struct directory_item);
    struct directory_item directories[howMany];
    for (j = 0; j < howMany; j++)
    {
        //TODO kdyz na konci clusteru fseekem posunout
        fread(&directories[j], sizeof(struct directory_item), 1, filePtr);
        if (strcmp( directories[j].name, dir_name) == 0)
        {
            copy_direct_item(&directories[j], found);
            return true;
        }
    }
    return false;
}


void process_path(char *path)
{
    if (strlen(path) == 0)
    {
        strcpy(path, root_item->name);
        return;
    }


    repair_back_slashes(path);
    int i, count = 0;
    // spoctu si pocet / - to oddelovace slov -> lehci reseni nez v split path reallocovat
    for (i = 0; i < strlen(path); i++)
    {
        if(path[i] == '/')
            count = count + 1;
    }

    char **path_parts = malloc((count +3) * sizeof(char *)); // array of words; 3- nahodne cislo pro zacatek
    int parts_num = split_path(path, path_parts);
    int length = sizeof(path) + 50;
    char *newPath = malloc (length);

    if (path[0] != '/')
    {
        strcpy(newPath, curr_path);
    }
    else
    {
        strcpy(newPath, root_item->name);
    }

    for (i = 0; i < parts_num; i++)
    {
        if (strcmp(path_parts[i], "..") == 0)
        {
            char *cutPath = malloc(sizeof(newPath));
            remove_path_last_part(cutPath, newPath);

            if(strcmp(cutPath, newPath) == 0)
            {
                strcpy(cutPath, root_item->name);
            }

            free(newPath);

            newPath = malloc(length * sizeof(char));
            strcpy(newPath, cutPath);
            free(cutPath);
        }
        else
        {
            if (strlen(path_parts[i]) > 0)
            {
                //TODO realloc string
                if (strlen(newPath) > 0 && newPath[strlen(newPath)-1] != '/')
                {
                    strcat(newPath, "/");
                }
                strcat(newPath, path_parts[i]);
            }
        }
    }

    strcpy(path, newPath);
    free(path_parts);
    free(newPath);
}