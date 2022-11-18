//
// Created by vondr on 06.10.2022.
//

#ifndef ZOS_SP_DIRCOMMANDS_H
#define ZOS_SP_DIRCOMMANDS_H

#include "../fat/fat.h"
/**
 * Done command of creating a directory
 * @param dirName   name of the directory to create
 */
void mkdir_command(char *dirName);



/**
 * Create new directory
 * @param dirName  name of the new directory
 * @param dir_where_create directory_structure where new directory is created
 * @return  result  EXISTS - 5, ENTERED_PATH_NOT_FOUND - 7, EXISTS - 5
 */
int make_directory(char * dirName, struct directory_item *dir_where_create);

/**
 * Perform rmdir command - remove given directory if it is empty
 * @param dir  directory to be removed
 */
void rmdir_command(char* dir);

/**
 * Find out if given directory is empty
 * @param dir   directory which is being checked if it is empty
 * @return      SUCCESS, NOT_EMPTY_DIR, PATH_NOT_FOUND, UNDEFINED_ERROR
 */
int is_empty(char* dir);

/**
 * Perform ls command -> print content of the directory
 * @param directory directory which content will be printed
 */
void ls_command(char* directory);

/**
 * Write out directory content
 * content is write out in format PATH ... and DIR ...
 * @param directory     directory to be writen out
 * @param look_from     from which directory write out
 * @return              SUCCESS, PATH_NOT_FOUND, UNDEFINED_ERROR,
 */
int write_out_dir(char* directory,  struct directory_item *look_from);

#endif //ZOS_SP_DIRCOMMANDS_H
