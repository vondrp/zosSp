#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input/inputHandler.h"
#include "fat/fat.h"

int main(int argc, char** argv) {
    const char *fat_name = argv[1];
    int size, i;

    // zformatovani FS
    if (argc == 3)
    {
        FILE *initFtr;
        struct boot_record init_br = {};
        struct directory_item init_root = {};
        int32_t *init_fat_table;

        printf("Jsem ve formatovani\n");
        initFtr = fopen(fat_name, "wb");

        if (initFtr == NULL)
        {
            printf("Pokus o vytvoření fat selhal");
            exit(2);
        }

        size = atoi(argv[2]);
        // inicializace super bloku
        init_boot_record(&init_br, size);

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
        for (i = 0; i < init_br.cluster_count; i++)
        {
            // * 8 - cluster_size is in 512B
            char cluster[init_br.cluster_size * 8];
            fwrite(cluster, init_br.cluster_size * 8 * sizeof (char), 1, initFtr);
        }

        // zavreni souboru inicializacniho souboru
        fclose(initFtr);
    }

    filePtr = fopen(fat_name, "rb+");

    if (filePtr == NULL){
        printf("Pokus o přečtení fat selhal\n");
        exit(2);
    }

    global_br = malloc (sizeof (global_br));
    root_item = malloc(sizeof (struct directory_item));

    // otisknutí superblock struktury z FS
    fread(global_br, sizeof(struct boot_record), 1, filePtr);

    // nacteni fat tabulky
    fat_table = malloc(global_br->cluster_count * sizeof(int32_t));
    for(i = 0; i < global_br->cluster_count; i++)
    {
        fread(&fat_table[i], sizeof(fat_table[i]), 1, filePtr);
    }

    // nacteni root directory item
    fread(root_item, sizeof (&root_item), 1, filePtr);

    // current directory is root at the start
    current_dir = root_item;

    // nastaveni soucasne cesty
    curr_path_max_length = 30; // znam delku soucacne cesty
    curr_path = malloc (sizeof(char) * curr_path_max_length);

    strcpy(curr_path, root_item->name);

    printf("Inicializace / čtení fat hotovo\n");
    printf("Vítejte, můžete psát příkazy.\n");
    process_input();

    free(curr_path);
    free(fat_table);
   // free(root_item);

    fclose(filePtr);
    return 0;
}