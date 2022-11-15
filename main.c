#include <stdio.h>
#include <stdlib.h>

#include "input/inputHandler.h"
#include "fat/fat.h"

int main(int argc, char** argv) {
    const char *fat_name = argv[1];
    int size, i;
    FILE *fptr;

    struct boot_record br = {};
    struct directory_item *drs;
    char **clusters;
    // zformatovani FS
    if (argc == 3)
    {
        printf("Jsem ve formatovani\n");
        fptr = fopen(fat_name, "wb");
        if (fptr == NULL){
            printf("Pokus o vytvoření fat selhal");
            exit(2);
        }

        size = atoi(argv[2]);
        printf("size: %d\n", size);
        // inicializace super bloku

        init_boot_record(&br, size);

        fwrite(&br, sizeof(br), 1, fptr); // posunuti fseek

        // zaplneni prazdna fat tabulka
        drs = malloc(br.fat_count * sizeof(struct directory_item));
        //struct directory_item *drs[br.fat_count];

        for (i = 0; i < br.fat_count; i++)
        {
            init_directory_item(&drs[i]);
            fwrite(&drs[i], sizeof(drs[i]), 1, fptr);
        }

        // inicializace clusters
        clusters = malloc (br.cluster_size * sizeof(char *));
        // vyplneni jakozto prazdne misto zbytek fat
        for (i = 0; i < br.cluster_count; i++)
        {
            clusters[i] = malloc(br.cluster_size * sizeof (char));
            //TODO prozkoumat z nejakeho duvodu je sizeof clusters[i] 8 a ne 512
            fwrite(clusters[i], br.cluster_size * sizeof (char), 1, fptr);
        }
    }
    else
    {
        fptr = fopen(fat_name, "rb");

        if (fptr == NULL){
            printf("Pokus o přečtení fat selhal\n");
            exit(2);
        }

        // otisknutí superblock struktury z FS
        fread(&br, sizeof(br), 1, fptr);

        // nacteni fat tabulky
        drs = malloc(br.fat_count * sizeof(struct directory_item));
        for (i = 0; i < br.fat_count; i++)
        {
            fread(&drs[i], sizeof(drs[i]), 1, fptr);
        }

        // precteni clusteru
        clusters = malloc (br.cluster_size * sizeof(char *));
        for (i = 0; i < br.cluster_count; i++)
        {
            fread(clusters[i], sizeof(clusters[i]), 1, fptr);
        }
    }
    printf("Inicializace / čtení fat hotovo");
    printf("Vítejte, můžete psát příkazy.\n");
    //process_input(&br, drs, clusters);

    fclose(fptr);
    free(drs);
    return 0;
}