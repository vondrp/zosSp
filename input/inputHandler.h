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
char *getLine();

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

#endif //ZOS_SP_INPUTHANDLER_H
