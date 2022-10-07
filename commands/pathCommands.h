//
// Created by vondr on 03.10.2022.
//

#ifndef ZOS_SP_PATHCOMMANDS_H
#define ZOS_SP_PATHCOMMANDS_H

/**
 * Perform pwd command
 * print out current working directory
 */
void pwd_command();

/**
 * Perform cd command
 * change current working directory
 * @param path  new working directory
 */
void cd_command(char *path);

#endif //ZOS_SP_PATHCOMMANDS_H
