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
            format_fs(size);
        }
        else
        {
            exit(2);
        }
    }
    open_fs();
    printf("Inicializace / čtení fat hotovo\n");
    printf("Vítejte, můžete psát příkazy.\n");
    process_input();

    close_fs();
    return 0;
}