//
// Created by vondr on 07.10.2022.
//

#ifndef ZOS_SP_FSCOMMANDS_H
#define ZOS_SP_FSCOMMANDS_H

#include <stdint.h>

/**
 * Will format the file of filesystem used by program
 * old data destroyed
 * @param size  new size - a positive integer with its unit info - allowed are kB, MB, GB
 */
void format_command (char *size_units);

/**
 * Process given unit to size in bytes
 * @param size_units    size together with units
 * @return              0 - wrong format of size_units or founded size in bytes
 */
int32_t process_units(char *size_units);

/**
 * Format used file system - FS must be closed - not used
 * @param size  size of fs file in bytes
 */
void format_fs(int32_t size);

/**
 * Close used filesystem file + free all global malloc memory
 */
void close_fs();

/**
 * Open fs file + load global data from it
 */
void open_fs();
#endif //ZOS_SP_FSCOMMANDS_H
