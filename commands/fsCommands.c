//
// Created by vondr on 07.10.2022.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "fsCommands.h"
#include "../fat/fat.h"
#include "../output/messages.h"
#include "../output/error.h"

void format_command(char *size_units)
{
    close_fs();
    int32_t size = process_units(size_units);
    if (size != 0)
    {
        format_fs(size);
    }
    open_fs();
}

void format_fs(int32_t size)
{
    FILE *initFtr;
    int i;
    struct boot_record init_br = {};
    struct directory_item init_root = {};
    int32_t *init_fat_table;

    initFtr = fopen(fs_filename, "wb");

    if (initFtr == NULL)
    {
        print_error_message(CANNOT_CREATE_FILE);
        exit(2);
    }

    // inicializace super bloku
    init_boot_record(&init_br, (int32_t)size);

    // ZAPIS boot record do souboru
    fwrite(&init_br, sizeof(struct boot_record), 1, initFtr); // posunuti fseek

    // zaplneni prazdna fat tabulka
    init_fat_table = malloc(init_br.cluster_count * sizeof(int32_t));

    // root directory
    init_directory_item(&init_root, "/\0", false, 0, 0);
    init_fat_table[0] = FAT_FILE_END; // na pozici nulteho clusteru se nachazi misto pro root objekty
    for (i = 1; i < init_br.cluster_count; i++)
    {
        init_fat_table[i] = FAT_UNUSED;
    }

    fwrite(init_fat_table, sizeof (int32_t), init_br.cluster_count, initFtr);

    // zapsani fat table do souboru

    // zapsani root directory itemu
    fwrite(&init_root, sizeof(struct directory_item), 1, initFtr);

    // inicializace clusters
    char* cluster = malloc(init_br.cluster_size);
    for (i = 0; i < init_br.cluster_count; i++)
    {
        // * 8 - cluster_size is in 512B
        fwrite(cluster, init_br.cluster_size, 1, initFtr);
    }
    free(cluster);

    // zavreni souboru inicializacniho souboru
    fclose(initFtr);
}


int32_t process_units(char *size_units)
{
    char *err_msg = "Parameter příkazu format musí být celé kladné číslo po němž bez mezery je dvojice znak kB, MB nebo GB\n";
    unsigned long size;

    if (strlen(size_units) < 3 || size_units[strlen(size_units)-1] != 'B')
    {
        printf("%s", err_msg);
        return 0;
    }

    int multiplier;
    int max;
    char unit = size_units[strlen(size_units)-2];
    if (unit == 'k')
    {
        multiplier = 1024;
        max = 2097151;
    }
    else if (unit == 'M')
    {
        multiplier = 1048576;
        max = 2047;
    }
    else if (unit == 'G')
    {
        multiplier = 1073741824;
        max = 1;
    }
    else
    {
        printf("%s", err_msg);
        return 0;
    }

    char *unit_ptr;
    size = strtol(size_units, &unit_ptr, 10);

    if (size <= 0)
    {
        printf("Velikost nebyla zadaná jako celé kladné číslo větší než 0\n");
        return 0;
    }
    else if (size > max) // there is max to prevent overflow of int32_t
    {
        printf("Přesažena maximální povolená velikost\n");
        printf("Max velikosti: 2097151kB |  2047MB | 1GB\n");
        return 0;
    }

    if (unit == 'k' && size <= 1)
    {
        printf("Minimální povolená velikost jsou 2kB\n");
        return 0;
    }

    size = size * multiplier;

    return (int32_t) size;
}

void open_fs()
{
    int i;
    filePtr = fopen(fs_filename, "rb+");

    if (filePtr == NULL){
        printf("Pokus o přečtení fs selhal\n");
        exit(2);
    }

    global_br = malloc (sizeof (struct boot_record));
    root_item = malloc(sizeof (struct directory_item));

    // read superblock
    fread(global_br, sizeof(struct boot_record), 1, filePtr);

    // load fat table
    fat_table = malloc(global_br->cluster_count * sizeof(int32_t));
    for(i = 0; i < global_br->cluster_count; i++)
    {
        fread(&fat_table[i], sizeof(int32_t), 1, filePtr);
    }

    // load root directory item
    fread(root_item, sizeof (struct directory_item), 1, filePtr);

    // current directory is root at the start
    current_dir = root_item;

    // load current path
    curr_path_max_length = 30; // znam delku soucacne cesty
    curr_path = malloc (sizeof(char) * curr_path_max_length);

    strcpy(curr_path, root_item->name);
}

void close_fs()
{
    fclose(filePtr);
    free(curr_path);
    free(fat_table);
    free(root_item);
}