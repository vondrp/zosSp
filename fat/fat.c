//
// Created by vondr on 15.11.2022.
//

#include "fat.h"

#include <string.h>
#include <bits/types/FILE.h>
#include <math.h>
#include <stdio.h>

const int32_t FAT_UNUSED = INT32_MAX - 1;
const int32_t FAT_FILE_END = INT32_MAX - 2;
const int32_t FAT_BAD_CLUSTER = INT32_MAX - 3;
const int32_t FAT_DIRECTORY = INT32_MAX - 4;

void init_boot_record(struct boot_record *bootRecord, int disk_size)
{
    bootRecord->disk_size = disk_size;
    //TODO doplnit vhodny pocet fat count
    bootRecord->fat_count = 10;
    bootRecord->fat1_start_address = sizeof (struct boot_record); // konec boot recordu
    bootRecord->data_start_address = sizeof (struct boot_record) + bootRecord->fat_count * sizeof (struct directory_item);

    bootRecord->cluster_size = 512 * 8; // 512 B
    int cluster_amount = disk_size - sizeof(struct boot_record) - bootRecord->fat_count * sizeof (struct directory_item);
    cluster_amount = (int)floor(cluster_amount / (double)bootRecord->cluster_size);

    printf("Cluster amount %d\n", cluster_amount);
    bootRecord->cluster_count = cluster_amount;
    strcpy(bootRecord->signature, "== ext ==");
}

void init_directory_item(struct directory_item* di)
{
    di->isFile = false;
    di->size = 0;
    di->start_cluster = FAT_UNUSED;
}

int find_free_fat_index(struct directory_item dis[], int fat_amount)
{
    int result = -1;
    int i;
    for (i = 0; i < fat_amount; i++)
    {

        if (dis[i].start_cluster == FAT_UNUSED){
            result = i;
            break;
        }
    }
    return result;
}