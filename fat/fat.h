//
// Created by vondr on 15.11.2022.
//
#include <stdint-gcc.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef ZOS_SP_FAT_H
#define ZOS_SP_FAT_H

extern const int32_t FAT_UNUSED;
extern const int32_t FAT_FILE_END;
extern const int32_t FAT_BAD_CLUSTER;
//extern const int32_t FAT_DIRECTORY;

extern FILE *filePtr;
extern int32_t *fat_table;
extern struct directory_item *root_item;
extern struct directory_item *current_dir;

extern char *curr_path;
extern int curr_path_max_length;
extern struct boot_record *global_br;

struct boot_record {
    char signature[9];              //login autora FS
    int32_t disk_size;              //celkova velikost VFS
    int32_t cluster_size;           //velikost clusteru
    int32_t cluster_count;          //pocet clusteru (stejny jako pocet polozek fat tabulky)
    int32_t fat1_start_address;	    //adresa pocatku FAT1 tabulky
    int32_t data_start_address;     //adresa pocatku datovych bloku (hl. adresar)
}; // 36B

//pokud bude ve FAT FAT_DIRECTORY, budou na disku v daném clusteru uloženy struktury o velikosti sizeof(directory) = 24B
struct directory_item {
    char name[13];                  //jméno souboru nebo adresáře ve tvaru 8.3'/0' nebo 12 + 1
    int32_t isFile;                    //identifikace zda je soubor (TRUE), nebo adresář (FALSE)
    int32_t size;                   //velikost položky, u adresáře 0 (bude zabirat jeden blok)
    int32_t start_cluster;          //počáteční cluster položky
};// 24B


void init_boot_record(struct boot_record *bootRecord, int disk_size);

void init_directory_item(struct directory_item* di, char name[13], bool isFile, int32_t size, int start_cluster);

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

void rewrite_fat();

void copy_direct_item(struct directory_item *source, struct directory_item *target);

struct directory_item find_dir(struct directory_item parent);

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
#endif //ZOS_SP_FAT_H
