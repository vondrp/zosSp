//
// Created by vondr on 15.11.2022.
//
#ifndef ZOS_SP_FAT_H
#define ZOS_SP_FAT_H

#include <stdint-gcc.h>
#include <stdbool.h>
#include <stdio.h>
#include "../input/checkInput.h"

extern const int32_t FAT_UNUSED;
extern const int32_t FAT_FILE_END;
extern const int32_t FAT_BAD_CLUSTER;
//extern const int32_t FAT_DIRECTORY;

extern FILE *filePtr;
extern char* fs_filename;
extern int32_t *fat_table;
extern struct directory_item *root_item;
extern struct directory_item *current_dir;

extern char *curr_path;
extern int curr_path_max_length;
extern struct boot_record *global_br;

struct boot_record {
    int32_t disk_size;              //celkova velikost VFS
    int32_t cluster_size;           //velikost clusteru
    int32_t cluster_count;          //pocet clusteru (stejny jako pocet polozek fat tabulky)
    int32_t fat1_start_address;	    //adresa pocatku FAT1 tabulky
    int32_t data_start_address;     //adresa pocatku datovych bloku (hl. adresar)
};

//pokud bude ve FAT FAT_DIRECTORY, budou na disku v daném clusteru uloženy struktury o velikosti sizeof(directory) = 24B
struct directory_item {
    char name[FILENAME_MAX_LENGTH + 1];    //jméno souboru nebo adresáře ve tvaru 8.3'/0' nebo 11 + 1
    int32_t isFile;                 //identifikace zda je soubor (TRUE), nebo adresář (FALSE)
    int32_t size;                   //velikost položky, u adresáře 0 (bude zabirat jeden blok)
    int32_t start_cluster;          //počáteční cluster položky
};// 24B

/**
 * Init boot record
 * @param bootRecord    boot record to be initialized
 * @param disk_size     size of filesystem in B
 */
void init_boot_record(struct boot_record *bootRecord, int disk_size);

/**
 * Init directory item
 * @param di        directory item to be initialized
 * @param name      name
 * @param isFile    is file
 * @param size      size of directory item
 * @param start_cluster    index of cluster, where directory item data starts
 */
void init_directory_item(struct directory_item* di, char name[13], bool isFile, int32_t size, int start_cluster);

/**
 * Find one free fat index
 * @return  free fat index or -1 if free index was not found
 */
int find_free_fat_index();

/**
 * Write new directory item to fs file - it will find free fat
 * @param parent
 * @param new_dir
 * @return
 */
int write_dir(struct directory_item *parent, struct directory_item *new_dir, struct directory_item *grandParent);

/**
 * Find out last cluster used by directory_item
 * @param dir   directory_item using cluster
 * @return      cluster index
 */
int last_cluster(struct directory_item *dir);

/**
 * Rewrite FAT table in the fs file
 */
void rewrite_fat();

/**
 * Copy directory item data
 * @param source    directory item to be copied
 * @param target    where copy of source is placed
 */
void copy_direct_item(struct directory_item *source, struct directory_item *target);

/**
 * Remove given directory item
 * @param parent        parent of the directory item (where it is placed)
 * @param toDestroy     directory item to be removed
 * @param grandparent   directory where parent is placed
 * @return
 */
int remove_dir_item(struct directory_item *parent, struct directory_item *toDestroy, struct directory_item *grandparent);

/**
 * Rewrite directory in his parent
 * @param child     directory item which value has been changed
 * @param parent    where child is located
 */
void upgrade_dir_item(struct directory_item *child, struct directory_item *parent);

/**
 * Find n free fat indexes
 * @param n         how many unused fat indexes to find
 * @param to_place  array of int where founded fat indexes are placed
 * @return          -1 - wrong parameters, SUCCESS, OUT OF FAT
 */
int find_free_fat_indexes(int32_t n, int32_t * to_place);

/**
 * Find out if two directory structures are same
 * IMPORTANT: compare according to their NAMES and START CLUSTERS
 * @param dir1  first directory structure
 * @param dir2  second directory structure
 * @return      true - are same, otherwise false
 */
bool equals(struct directory_item dir1, struct directory_item dir2);


/**
 * Remove given directory item from directory where it is place
 * - DOES NOT CLEAR directory item from FAT table
 * @param parent        directory where directory item to be removed is placed
 * @param toRemove      directory item to be removed from his parent directory
 * @param grandparent   grandparent directory of directory item to remove
 */
void remove_from_directory(struct directory_item *parent, struct directory_item *toRemove, struct directory_item *grandparent);

/**
 * Clear given directory item from FAT table
 * @param toClear   directory item to be removed from fat table
 */
void clear_from_fat(struct directory_item *toClear);

/**
 * Rename given directory_item
 * @param child     directory item to be rename
 * @param parent    parent directory of the directory item to be rename
 * @param newName   new name of the directory item
 */
void rename_dir(struct directory_item *child, struct directory_item *parent, char* newName);


/**
 * Find n free fat indexes
 * @param n         how many unused fat indexes to find
 * @param to_place  array where founded fat indexes are placed
 * @return          ERROR_INTERNAL - wrong parameters, SUCCESS, OUT OF FAT
 */
int find_neighboring_free_fat_indexes(int32_t n, int32_t *to_place);

/**
 * Check if given directory exists
 * @param directory     directory to be checked if exists
 * @param look_from     from which directory we are looking for directory
 * @param last_part     return last directory_item
 * @return              EXISTS, PATH_NOT_FOUND (NOT EXISTS)
 */
int directory_exists(char *directory, struct directory_item *look_from, struct directory_item *last_part);

/**
 * Find out if directory exists in given directory_item
 * @param directory_parent  place where look for directory
 * @param dir_name          name of the directory to found
 * @param found             safe reference to founded directory is success
 * @return                  true - found, false - not in directory_parent
 */
bool is_in_dir(struct directory_item *directory_parent, char dir_name[13], struct  directory_item *found);
#endif //ZOS_SP_FAT_H
