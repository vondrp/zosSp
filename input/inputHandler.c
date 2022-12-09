//
// Created by vondr on 01.10.2022.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "inputHandler.h"

#include "../commands/fileCommands.h"
#include "../commands/dirCommands.h"
#include "../commands/pathCommands.h"
#include "../commands/fsCommands.h"

#include "checkInput.h"


#define max(x,y) (((x) >= (y)) ? (x) : (y))

char *get_line(FILE *file)
{
    char * line = malloc(INPUT_LENGTH * sizeof (char)), * linep = line;
    size_t lenmax = INPUT_LENGTH * sizeof (char), len = lenmax;
    char c;

    if(line == NULL)
        return NULL;

    for(;;) {
        c = (char) fgetc(file);
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

void sentence_to_words(char *console_input, char** words, int *last_words_amount)
{
    int words_amount = DEFAULT_WORDS_AMOUNT;
    int i,j,ctr, len_max;
    j = 0;
    ctr = 0;
    len_max = DEFAULT_WORDS_LENGTH;
    for (i = 0; i <=  (strlen(console_input)); i++) {

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

            // to ignore multiple white spaces
            if (j != 0)
            {
                words[ctr][j] = '\0';
                ctr++;  //for next word
            }

            // realloc more space for words if necessary
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

    if (*last_words_amount > ctr)
    {
        for (i = ctr + 1; i <= *last_words_amount; i++)
        {
            for (j = 0; j < strlen(words[i]); j++)
                words[i][j] = '\0';
        }
    }

    *last_words_amount = ctr;
}

void process_input()
{
    char *console_input; //console input from user
    char **words;       // console input separated into words
    int words_amount = DEFAULT_WORDS_AMOUNT;

    words = malloc(words_amount * sizeof(char *)); // array of words
    char end_input[] = "end";
    int last_words_amount;

    last_words_amount = 0; // amount of words used in last console input
    do {
        printf("%s> ", curr_path);
        console_input = get_line(stdin);

        sentence_to_words(console_input, words, &last_words_amount);

        /*
        for (i = 0; i < last_words_amount; i++)
        {
            printf("%s %llu\n", words[i], strlen(words[i]));
        }*/
        call_commands(words);
    } while(strcmp(words[0], end_input) != 0);

    free_words_array(words, words_amount);
    free(console_input);
}

void call_commands(char** words)
{
    if (strcmp(words[0], "cp") == 0)
    {
        cp_command(words[1], words[2]);
    }
    else if (strcmp(words[0], "mv") == 0)
    {
        mv_command(words[1], words[2]);
    }
    else if (strcmp(words[0], "rm") == 0)
    {
        rm_command(words[1]);
    }
    else if (strcmp(words[0], "mkdir") == 0)
    {
        mkdir_command(words[1]);
    }
    else if (strcmp(words[0], "rmdir") == 0)
    {
        rmdir_command(words[1]);
    }
    else if (strcmp(words[0], "ls") == 0)
    {
        ls_command(words[1]);
    }
    else if (strcmp(words[0], "cat") == 0)
    {
        cat_command(words[1]);
    }
    else if (strcmp(words[0], "cd") == 0)
    {
        cd_command(words[1]);
    }
    else if (strcmp(words[0], "pwd") == 0)
    {
        pwd_command();
    }
    else if (strcmp(words[0], "load") == 0)
    {
        load_command(words[1]);
    }
    else if (strcmp(words[0], "incp") == 0)
    {
        incp_command(words[1], words[2]);
    }
    else if (strcmp(words[0], "pathPr") == 0)
    {
        process_path(words[1]);
    }
    else if (strcmp(words[0], "info") == 0)
    {
        info_command(words[1]);
    }
    else if (strcmp(words[0], "outcp") == 0)
    {
        outcp_command(words[1], words[2]);
    }
    else if (strcmp(words[0], "defrag") == 0)
    {
        defrag_command(words[1]);
    }
    else if (strcmp(words[0], "format") == 0)
    {
        format_command(words[1]);
    }
}