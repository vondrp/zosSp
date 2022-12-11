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
 * @return			SUCCESS or error type of - TOO_LONG_FILENAME
 */
int check_filename_input(char *string);

/**
 * find all \ and replace them with /
 * @param string    path
 */
void repair_back_slashes(char * string);

/**
 * From give path remove its last path (part after last /)
 * @param remainingPath place where the remaining path will be placed
 * @param fullPath  full path
 */
void remove_path_last_part(char *remainingPath, char *fullPath);

/**
 * Check if given path is outside directory
 * @param path  path to be checked
 * @return  true (1) or false(0)
 */
int is_outside_directory(char *path);

 /**
  * Method used for splitting sentence to to array of words
  * @param path                 sentence input
  * @param path_parts           array of strings, where result is stored
  * @param expected_path_parts  on how many parts is path expected to be split on
  * @return                     amount of founded parts
  */
int split_path(char *path, char** path_parts, int expected_path_parts);

/**
 * Method which proceed give path - create complete path from directory, not check if actually exists
 * @param path  path to be checked and modified
 */
void process_path(char *path);

#endif //ZOSSP_CHECKINPUT_H
