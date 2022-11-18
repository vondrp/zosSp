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
 * Write new directory to file
 * @param parent
 * @param new_dir
 * @return
 */
int write_dir(struct directory_item *parent, struct directory_item *new_dir);

int last_cluster(struct directory_item *dir);

void rewrite_fat();

void copy_direct_item(struct directory_item *source, struct directory_item *target);

struct directory_item find_dir(struct directory_item parent);
#endif //ZOS_SP_FAT_H