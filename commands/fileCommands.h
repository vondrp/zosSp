//
// Created by vondr on 06.10.2022.
//

#ifndef ZOS_SP_FILECOMMANDS_H
#define ZOS_SP_FILECOMMANDS_H

/**
 * Will perform cp command - which will copy content of source file to a new file target file
 * information's about command success/failure is printed to console
 * @param source_file   source file, from which the content is copied
 * @param target_file   target file to which se source file content will by copied
 */
void cp_command (char *source_file, char *target_file);

/**
 * Copy content of source file to a new file target file
 * @param source_file   source file, from which the content is copied
 * @param target_file   target file to which se source file content will by copied
 * @return              SUCCESS - 0, SOURCE_FILE_NOT_FOUND - 1 (source file not found),
 *                      PATH_NOT_FOUND - 2 (path to the target file was not found),
 */
int copy_file (char *source_file, char *target_file);

/**
 * Perform rm command - delete given file
 * result is printed to the console
 * @param file  file to be deleted
 */
void rm_command(char *file);

/**
 * Delete given file
 * @param file file to ve deleted
 * @return  SUCCESS - 0, REMOVE_ERROR - 150, FILE_NOT_FOUND - 3
 */
int remove_file(char * file);

/**
 * Perform mv command - move given file to give target directory or rename file
 * result is printed to the console
 * @param source_file   source file to be moved
 * @param target_path   path to the target directory
 */
void mv_command (char *source_file, char *target_path);


/**
 * Perform cat command - print content of the file to the console
 * @param filename      filename of the file, which content is going to be printed
 */
void cat_command(char* filename);

/**
 * Perform load command -> load script file with commands
 * one line - one command, only command load with the same scriptFile will not be done
 * @param scriptFile    file containing commands to be executed
 */
void load_command(char* scriptFile);

/**
 * From outside of the FileSystem load file and place it to the file system
 * @param outsideFile   outside file
 * @param toPlace       where to place file
 */
void incp_command(char* outsideFile, char* toPlace);

/**
 * Perform info command - print information about the file to the console
 * @param filename      filename of the file, which info is going to be printed
 */
void info_command(char* filename);

/**
 * Copy file from filesystem to outside
 * @param file      file to be copies
 * @param toPlace   place where the file will be placed
 */
void outcp_command(char* file, char* toPlace);


/**
 * File defragmentation
 * - all its data blocks align next to each other
 * @param filename  path to the file to be defragmentation
 */
void defrag_command(char *filename);

#endif //ZOS_SP_FILECOMMANDS_H
