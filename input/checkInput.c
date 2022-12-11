//
// Created by vondr on 24.09.2022.
//
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#include "checkInput.h"
#include "../output/error.h"

#define FILENAME_MAX_LENGTH 11

char* get_filename(char *string_path) {
    repair_back_slashes(string_path);
    char *dot = strrchr(string_path, '/');

    if (!dot) return string_path;
    //if(!dot || strcmp(dot, string_path) == 0) return string_path;

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

int is_outside_directory(char *path)
{
    /*struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);*/
    struct stat info;

    int statRC = stat( path, &info );
    if( statRC != 0 )
    {
        if (errno == ENOENT)  { return 0; } // something along the path does not exist
        if (errno == ENOTDIR) { return 0; } // something in path prefix is not a dir
        return -1;
    }

    return ( info.st_mode & S_IFDIR ) ? 1 : 0;
}


int split_path(char *path, char** path_parts, int expected_path_parts)
{
    int path_amount = expected_path_parts;
    int i,j,ctr, len_max;
    j = 0;
    ctr = 0;
    len_max = FILENAME_MAX_LENGTH;
    for (i = 0; i <= (strlen(path)); i++)
    {
        if (j == 0)
        {
            len_max = FILENAME_MAX_LENGTH;
            path_parts[ctr] = malloc(len_max * sizeof(char *));
        }
        else if (j >= len_max)
        {
            len_max = len_max * 2;
            path_parts[ctr] = realloc(path_parts[ctr], len_max * sizeof (char));
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
                path_amount = path_amount * 2;
                /*unsigned long currSize = 0;
                for (j = 0; j < ctr; j++)
                    currSize = currSize + strlen(path_parts[j]);
                */
                //TODO je treba reallocovat jinak
                printf("Jsem pred realloc ve split path path amounts %d\n", path_amount);
                path_parts = realloc(path_parts, (path_amount) * sizeof(*path_parts));

                //path_parts = realloc(path_parts, path_amount * len_max * sizeof(char));
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

void process_path(char *path)
{
    if (strlen(path) == 0)
    {
        strcpy(path, root_item->name);
        return;
    }

    repair_back_slashes(path);
    int i, count = 0;
    // count / - -> easier solution than do realloc in v split path
    for (i = 0; i < strlen(path); i++)
    {
        if(path[i] == '/')
            count = count + 1;
    }

    // add parts of curr path (we can be using and can be longer than given path
    for (i = 0; i < strlen(curr_path); i++)
    {
        if(path[i] == '/')
            count = count + 1;
    }

    count = count + 2; // + 1 before first / and + 1 after last /

    unsigned long length = (strlen(path) + strlen(curr_path)) * sizeof(char);
    char *newPath = malloc (length);

    char **path_parts = malloc((count) * length); // array of words;
    int parts_num = split_path(path, path_parts, count);

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
            char *cutPath = malloc(strlen(newPath) * sizeof(char));
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
        else if (strcmp(path_parts[i], ".") == 0)
        {
            continue;
        }
        else
        {
            if (strlen(path_parts[i]) > 0)
            {
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