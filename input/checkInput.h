//
// Created by vondr on 24.09.2022.
//

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
 * @return              EXISTS, PATH_NOT_FOUND (NOT EXISTS), UNDEFINED_ERROR
 */
int directory_exists(char *directory);

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
#endif //ZOSSP_CHECKINPUT_H
