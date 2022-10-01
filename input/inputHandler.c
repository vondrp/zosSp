//
// Created by vondr on 01.10.2022.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "inputHandler.h"

#include "../commands/copyFile.h"
#include "../commands/removeFile.h"
#include "../commands/moveFile.h"

char *getLine(void)
{
    char * line = malloc(INPUT_LENGTH), * linep = line;
    size_t lenmax = INPUT_LENGTH, len = lenmax;
    int c;

    if(line == NULL)
        return NULL;

    for(;;) {
        c = fgetc(stdin);
        if(c == EOF)
            break;

        if(--len == 0) {
            len = lenmax;
            char * linen = realloc(linep, lenmax *= 2);

            if(linen == NULL) {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        if((*line++ = c) == '\n')
            break;
    }
    *line = '\0';
    return linep;
}

void free_words_array(char *a[], int length)
{
    int i, j;
    for (i = 0; i < length; i++) {
        if (a[i] != NULL) {
            for (j = i + 1; j < length; j++) {
                if (a[j] == a[i]) {
                    a[j] = NULL;
                }
            }
            free(a[i]);
        }
    }
}

void process_input()
{
    char *console_input;
    char **words;
    int words_amount = DEFAULT_WORDS_AMOUNT;

    words = malloc(words_amount * sizeof(char *));
    char end_input[] = "end";
    int i,j,ctr, len_max;

    do {
        console_input = getLine();
        // fgets(console_input, sizeof console_input, stdin);

        j = 0;
        ctr = 0;
        len_max = DEFAULT_WORDS_LENGTH;
        for (i = 0; i <= (strlen(console_input)); i++) {

            if (j == 0)
            {
                len_max = DEFAULT_WORDS_LENGTH;
                words[ctr] = malloc(len_max * sizeof(char *));
            }
            else if (j >= len_max)
            {
                len_max = len_max * 2;
                words[ctr] = realloc(words[ctr], len_max * sizeof (char *));
            }

            // if space or NULL found, assign NULL into newString[ctr]
            //printf("Pozice %d, znak %c, isspace %d\n", i, console_input[i], isspace(console_input[i]));
            if (isspace(console_input[i]) != 0) {
                //newString[ctr][j] = '\0';
                words[ctr][j] = '\0';
                ctr++;  //for next word

                if (ctr >= words_amount)
                {
                    words_amount = words_amount * 2;
                    words = realloc(words, words_amount * sizeof (char *));
                }
                j = 0;    //for next word, init index to 0
            } else {
                words[ctr][j] = console_input[i];
                j++;
            }
        }

        for (i = 0; i < ctr; i++)
        {
            printf("%s %llu\n", words[i], strlen(words[i]));
        }

        // printf("%d", strcmp(newString[0], "cp"));
        if (strcmp(words[0], "cp") == 0)
        {
            cp_command(words[1], words[2]);
        }
        else if (strcmp(words[0], "rm") == 0)
        {
            rm_command(words[1]);
        }
        else if (strcmp(words[0], "mv") == 0)
        {
            mv_command(words[1], words[2]);
        }
    } while(strcmp(words[0], end_input) != 0);

    free_words_array(words, words_amount);
    free(console_input);
}