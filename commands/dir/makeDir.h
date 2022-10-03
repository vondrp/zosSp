//
// Created by vondr on 02.10.2022.
//

#ifndef ZOS_SP_MAKEDIR_H
#define ZOS_SP_MAKEDIR_H

/**
 * Done command of creating a directory
 * @param dirName   name of the directory to create
 */
void mkdir_command(char *dirName);

/**
 * Create new directory
 * @param dirName  name of the new directory
 * @return  result
 */
int make_directory(char * dirName);
#endif //ZOS_SP_MAKEDIR_H