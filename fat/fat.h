//
// Created by vondr on 15.11.2022.
//
#include <stdint-gcc.h>
#include <stdbool.h>

#ifndef ZOS_SP_FAT_H
#define ZOS_SP_FAT_H

extern const int32_t FAT_UNUSED;
extern const int32_t FAT_FILE_END;
extern const int32_t FAT_BAD_CLUSTER;
extern const int32_t FAT_DIRECTORY;

struct boot_record {
    char signature[9];              //login autora FS
    int32_t disk_size;              //celkova velikost VFS
    int32_t cluster_size;           //velikost clusteru
    int32_t cluster_count;          //pocet clusteru
    int32_t fat_count;        	    //pocet polozek kazde FAT tabulce
    int32_t fat1_start_address;	    //adresa pocatku FAT1 tabulky
    //int32_t fat2_start_address;     //adresa pocatku FAT2 tabulky
    int32_t data_start_address;     //adresa pocatku datovych bloku (hl. adresar)
}; // 36B

//pokud bude ve FAT FAT_DIRECTORY, budou na disku v daném clusteru uloženy struktury o velikosti sizeof(directory) = 24B
struct directory_item {
    char name[13];                  //jméno souboru nebo adresáře ve tvaru 8.3'/0' nebo 12 + 1
    bool isFile;                    //identifikace zda je soubor (TRUE), nebo adresář (FALSE)
    int32_t size;                   //velikost položky, u adresáře 0 (bude zabirat jeden blok)
    int32_t start_cluster;          //počáteční cluster položky
};// 24B


void init_boot_record(struct boot_record *bootRecord, int disk_size);

void init_directory_item(struct directory_item* di);

int find_free_fat_index(struct directory_item dis[], int fat_amount);
#endif //ZOS_SP_FAT_H
