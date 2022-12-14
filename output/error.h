//
// Created by vondr on 24.09.2022.
//

#ifndef ZOSSP_ERROR_H
#define ZOSSP_ERROR_H

//#define     MAX_CHAR 11 //can be 12 symbols but the 11 symbol is for \0

#define		SUCCESS 0
#define     SOURCE_FILE_NOT_FOUND 1
#define     TARGET_PATH_NOT_FOUND 2
#define     FILE_NOT_FOUND 3
#define     PATH_NOT_FOUND 4
#define     EXISTS 5
#define     NOT_EMPTY_DIR 6
#define     ENTERED_PATH_NOT_FOUND 7
#define     DIR_PATH_NOT_FOUND 10
#define     CANNOT_CREATE_FILE 11
#define     REMOVE_ERROR 150
#define     UNDEFINED_ERROR 180
#define     TOO_LONG_FILENAME 200
#define		WRONG_PARAMETERS_IN_FUNCTION_RETURNING_UNSIGNED_CHAR 300
#define     MEMORY_PROBLEMS 350

#define     ERROR_INTERNAL 351

#define     OUT_OF_FAT 8
#define     DIR_OUT_OF_SPACE 9

#endif //ZOSSP_ERROR_H
