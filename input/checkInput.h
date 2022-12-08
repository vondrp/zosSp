//
// Created by vondr on 24.09.2022.
//

#include <stdbool.h>
#include "../fat/fat.h"

#ifndef ZOSSP_CHECKINPUT_H
#define ZOSSP_CHECKINPUT_H

/**
 * From the given string path return filename (expect it to be at the end)
 * @param string_path   path with filename at the end
 * @return              filename
 */
char *get_filename(char *string_path);

/**
 * check if input is alright
 * @param string	fileName with his path
 * @return			SUCCESS or error type of - MUST FINISH WHEN DECIDE WHAT TO WRITE
 */
int check_filename_input(char *string);

/**
 * find all \ and replace them with /
 * @param string    path
 */
void repair_back_slashes(char * string);

/**
 * Check if given directory exists
 * @param directory     directory to be checked if exists
 * @param look_from     from which directory we are looking for directory
 * @param last_part     return last directory_item
 * @return              EXISTS, PATH_NOT_FOUND (NOT EXISTS)
 */
int directory_exists(char *directory, struct directory_item *look_from, struct directory_item *last_part);

/**
 * From give path remove its last path (part after last /)
 * @param remainingPath place where the remaining path will be placed
 * @param fullPath  full path
 */
void remove_path_last_part(char *remainingPath, char *fullPath);

/**
 * Check if given path is directory
 * @param path  path to be checked
 * @return  true (1) or false(0)
 */
int is_directory(char *path);


 /**
 * Method used for splitting sentence to to array of words
 * @param path          sentence input
 * @param path_parts     array of strings, where result is stored
 * @return               amount of founded parts
 */
int split_path(char *path, char** path_parts);

/**
 * Find out if directory exists in given directory_item
 * @param directory_parent  place where look for directory
 * @param dir_name          name of the directory to found
 * @param found             safe reference to founded directory is success
 * @return                  true - found, false - not in directory_parent
 */
bool is_in_dir(struct directory_item *directory_parent, char dir_name[13], struct  directory_item *found);

/**
 * Method which proceed give path - create complete path from directory, not check if actually exists
 * @param path  path to be checked and modified
 */
void process_path(char *path);

#endif //ZOSSP_CHECKINPUT_H
