//
// Created by vondr on 24.09.2022.
//

#ifndef ZOSSP_CHECKINPUT_H
#define ZOSSP_CHECKINPUT_H

char *getFilename(char *string_path);

/**
 * check if input is alright
 * @param string	fileName with his path
 * @return			SUCCESS or error type of - MUST FINISH WHEN DECIDE WHAT TO WRITE
 */
int checkFilenameInput(char *string);

/**
 * find all \ and replace them with /
 * @param string    path
 */
void repairBackSlashes(char * string);

/**
 * Check if given directory exists
 * @param directory     directory to be checked if exists
 * @return              EXISTS, PATH_NOT_FOUND (NOT EXISTS), UNDEFINED_ERROR
 */
int directoryExists(char *directory);

void removePathLastPart(char *remainingPath, char *fullPath);

/**
 * Check if given path is directory
 * @param path  path to be checked
 * @return  true (1) or false(0)
 */
int is_directory(char *path);
#endif //ZOSSP_CHECKINPUT_H
