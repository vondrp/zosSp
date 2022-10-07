//
// Created by vondr on 01.10.2022.
//

#ifndef ZOS_SP_INPUTHANDLER_H
#define ZOS_SP_INPUTHANDLER_H

#define DEFAULT_WORDS_LENGTH 20
#define DEFAULT_WORDS_AMOUNT 12
#define INPUT_LENGTH 100

/**
 * Method for getting line from the console
 * given line is dynamically allocated
 * -> when output no longer needed call free
 * @return  pointer to the dynamically allocated line
 */
char *get_line(FILE *file);

/**
 * Free array of dynamically allocated strings
 * @param a         array of dynamically allocated strings
 * @param length    length of the array
 */
void free_words_array(char *a[], int length);

/**
 * Method for processing user input
 */
void process_input();


/**
 * Method used for splitting sentence to to array of words
 * @param console_input     sentence input
 * @param words             array of strings, where result is stored
 * @param last_words_amount how many words were used during last usage (for removing old words)
 */
void sentence_to_words(char *console_input, char** words, int *last_words_amount);

/**
 * Method used for calling commands
 * @param words array of words (it is expected that 1 word is command)
 */
void call_commands(char** words);

#endif //ZOS_SP_INPUTHANDLER_H
