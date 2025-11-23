#ifndef WORDLE_H
#define WORDLE_H

#include <stdbool.h>

#define WORD_LEN 5
#define MAX_WORDS 20000
#define MAX_GUESSES 6

typedef enum { GRAY = 0, YELLOW = 1, GREEN = 2 } Color;

int load_dictionary(const char *filename);
bool is_valid_word(const char *w);
void free_dictionary();

void compute_feedback(const char *guess, const char *target, Color colors[]);
void print_feedback(const char *guess, Color colors[]);

int solver_user_feedback();
void human_play();

#endif
