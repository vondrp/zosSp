//
// Created by vondr on 01.10.2022.
//
#include <stdio.h>

#include "messages.h"
#include "error.h"

void print_error_message(int error)
{
    switch (error)
    {
        case SUCCESS:
            printf("OK");
            break;
        case FILE_NOT_FOUND:
            printf("FILE NOT FOUND (není zdroj)");
            break;
        case PATH_NOT_FOUND:
            printf("PATH NOT FOUND (neexistuje cílová cesta)");
            break;
        case EXISTS:
            printf("EXISTS (nelze založit, již existuje");
            break;
        case NOT_EMPTY:
            printf("NOT EMPTY ((adresář obsahuje podadresáře, nebo soubory)");
            break;
        case TOO_LONG_FILENAME:
            printf("MAXIMAL LENGTH OF FILENAME IS 12 SYMBOLS (maximální délka jména souboru je 12 znaků)");
            break;
        case REMOVE_ERROR:
            printf("FILE REMOVING FILED (soubor byl nalezen, ale smazání selhalo)");
            break;
        default:
            printf("NOT SPECIFIED RESULT");
            break;

    }
    printf("\n");
}