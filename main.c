#include <stdio.h>
#include <stdlib.h>

#include "input/inputHandler.h"
#include "fat/fat.h"
#include "commands/fsCommands.h"

int main(int argc, char** argv) {

    if (argc < 2)
    {
        printf("chybí jméno souboru\n");
        return 0;
    }

    fs_filename = argv[1];
    // form FS
    if (argc == 3)
    {
        int32_t size = process_units(argv[2]);
        if (size != 0)
        {
            printf("Souborový systém se bude nyní formátovat na velikost %s\n", argv[2]);
            format_fs(size);
        }
        else
        {
            exit(2);
        }
    }
    open_fs();
    printf("Načtení souborového systému je hotovo\n");
    printf("Vítejte, můžete psát příkazy.\n");
    process_input();

    close_fs();
    return 0;
}