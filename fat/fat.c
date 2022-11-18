//
// Created by vondr on 15.11.2022.
//

#include "fat.h"
#include "../utils/error.h"

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const int32_t FAT_UNUSED = INT32_MAX - 1;
const int32_t FAT_FILE_END = INT32_MAX - 2;
const int32_t FAT_BAD_CLUSTER = INT32_MAX - 3;
//const int32_t FAT_DIRECTORY = INT32_MAX - 4;

FILE *filePtr;
int32_t *fat_table;
struct directory_item *root_item;
struct directory_item *current_dir;

char *curr_path;
int curr_path_max_length;
struct boot_record *global_br;


void init_boot_record(struct boot_record *bootRecord, int disk_size)
{
    unsigned long sizeOfFat;
    int cluster_amount;
    strcpy(bootRecord->signature, "== ext ==");
    bootRecord->disk_size = disk_size;
    bootRecord->cluster_size = 512; // 512 B - do bitu - 4096

    // z celkove velikosti odecist velikost boot record + 1 directory item (root umisten za fav tabulkou)
    cluster_amount = disk_size - (int)sizeof(struct boot_record) - (int) sizeof (struct directory_item);
    cluster_amount = ceil(cluster_amount / 512.0);
    bootRecord->cluster_count = cluster_amount;

    sizeOfFat = cluster_amount * sizeof (int32_t);
    bootRecord->fat1_start_address = sizeof (struct boot_record); // konec boot recordu
    // data start after boot record, fat table and root directory item
    bootRecord->data_start_address = sizeof(struct boot_record) + sizeOfFat + sizeof (struct directory_item);
}

void init_directory_item(struct directory_item* di, char name[13], bool isFile, int32_t size, int start_cluster)
{
    strcpy(di->name, name);
    di->isFile = isFile;
    di->size = size;
    di->start_cluster = start_cluster;
}

int find_free_fat_index()
{
    int result = -1;
    int i;
    for (i = 0; i < global_br->cluster_size; i++)
    {
        if (fat_table[i] == FAT_UNUSED)
        {
            result = i;
            break;
        }
    }
    return result;
}

int write_dir(struct directory_item *parent, struct directory_item *new_dir, struct directory_item *grandParent)
{
    int fat_index;

    int usedSpace = parent->size % global_br->cluster_size;
    int cluster_index = last_cluster(parent);

    // rozsizeni souboru do dalsiho clusteru
    if (usedSpace + sizeof (struct directory_item) > global_br->cluster_size)
    {
        fat_index = find_free_fat_index();
        if (fat_index == -1)
        {
            return OUT_OF_FAT;
        }

        fat_table[cluster_index] = fat_index;
        fat_table[fat_index] = FAT_FILE_END;

        cluster_index = fat_index;
        usedSpace = 0;
        rewrite_fat();
    }

    long clusterStart = global_br->data_start_address
            + cluster_index * global_br->cluster_size
            + usedSpace;

    // zapsani noveho directory
    fseek(filePtr, clusterStart, SEEK_SET);
    fwrite(new_dir, sizeof (struct directory_item), 1, filePtr);

    parent->size = parent->size + (int32_t)sizeof(struct directory_item);

    // upgrade rodice
    upgrade_dir_item(parent, grandParent);

    // reload current dir
    if (strcmp(parent->name, current_dir->name) == 0)
    {
        copy_direct_item(parent, current_dir);
    }
    else if (strcmp(parent->name, root_item->name) == 0)
    {
        copy_direct_item(parent, root_item);
    }
    return SUCCESS;
}


void upgrade_dir_item(struct directory_item *child, struct directory_item *parent) {
    long clusterStart;
    if (strcmp(child->name, "/") == 0) //rodic je koren
    {
        clusterStart = global_br->data_start_address - sizeof(struct directory_item);
        fseek(filePtr, clusterStart, SEEK_SET);
        fwrite(child, sizeof(struct directory_item), 1, filePtr);
    }
    else
    {
        clusterStart = global_br->data_start_address + parent->start_cluster * global_br->cluster_size;
        fseek(filePtr, clusterStart, SEEK_SET);
        unsigned long grandParentHowMany = parent->size / sizeof(struct directory_item);

        struct directory_item grandpaDirectories[grandParentHowMany];
        int howFarAway = 0;
        int placeManCluster = parent->start_cluster;
        for (int j = 0; j < grandParentHowMany; j++) {
            fread(&grandpaDirectories[j], sizeof(struct directory_item), 1, filePtr);
            if (strcmp(grandpaDirectories[j].name, child->name) == 0) {
                howFarAway = j;
                break;
            }
        }

        clusterStart = global_br->data_start_address + placeManCluster * global_br->cluster_size
                       + howFarAway * sizeof(struct directory_item);
        fseek(filePtr, clusterStart, SEEK_SET);
        fwrite(child, sizeof(struct directory_item), 1, filePtr);
    }
}

int remove_dir(struct directory_item *parent, struct directory_item *toDestroy, struct directory_item *grandparent)
{
    // odstraneni directory z fat tabulky
    int fat_index = toDestroy->start_cluster;
    fat_table[fat_index] = FAT_UNUSED;
    rewrite_fat();
    int i = 0;
    int parentSpace = parent->size % global_br->cluster_size;

    unsigned long howMany = parent->size / sizeof(struct directory_item);
    int clusterSize = global_br->data_start_address + parent->start_cluster * sizeof(global_br->cluster_size);
    fseek(filePtr, clusterSize, SEEK_SET);

    struct directory_item directoryItems[howMany - 1]; // -1 - jeden odebereme
    while (i < howMany-1)
    {
        struct directory_item test = {};
        fread(&test, sizeof(struct directory_item), 1, filePtr);
        if (strcmp(test.name, toDestroy->name) != 0) // neni stejne
        {
            copy_direct_item(&test, &directoryItems[i]);
            i++;
        }
    }
    fseek(filePtr, clusterSize, SEEK_SET);

    //prepsani polozek clusteru
    for (i = 0; i < howMany-1; i++)
    {
        fwrite(&directoryItems[i], sizeof (struct directory_item), 1, filePtr);
    }

    parent->size = parent->size - (int32_t)sizeof(struct directory_item);

    // upgrade zaznamu parent v grandparent
    upgrade_dir_item(parent, grandparent);

    // reload current dir
    if (strcmp(parent->name, current_dir->name) == 0)
    {
        copy_direct_item(parent, current_dir);
    }
    else if (strcmp(parent->name, root_item->name) == 0)
    {
        copy_direct_item(parent, root_item);
    }
    return SUCCESS;
}


int last_cluster(struct directory_item *dir)
{
    int nextCluster = fat_table[dir->start_cluster];

    int previous = dir->start_cluster;
    while (nextCluster != FAT_FILE_END)
    {
        previous = nextCluster;
        nextCluster = fat_table[nextCluster];
    }

    return previous;
}

void rewrite_fat()
{
    fseek(filePtr, global_br->fat1_start_address, SEEK_SET);
    fwrite(fat_table, sizeof (int32_t), global_br->cluster_count, filePtr);
}

void copy_direct_item(struct directory_item *source, struct directory_item *target)
{
    strcpy(target->name, source->name);
    target->isFile = source->isFile;
    target->size = source->size;
    target->start_cluster = source->start_cluster;
}