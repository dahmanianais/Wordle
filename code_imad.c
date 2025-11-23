#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include "wordle.h"

char **dictionary = NULL;
int dict_size = 0;

/* ---------------- Dictionnaire ---------------- */
int load_dictionary(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
    {
        perror("open dictionary");
        return -1;
    }

    dictionary = malloc(sizeof(char *) * MAX_WORDS);
    if (!dictionary)
    {
        fclose(f);
        return -1;
    }

    char buf[128];
    dict_size = 0;

    while (fgets(buf, sizeof(buf), f))
    {
        buf[strcspn(buf, "\r\n")] = 0;
        for (size_t k = 0; k < strlen(buf); k++)
            buf[k] = tolower((unsigned char)buf[k]);
        if ((int)strlen(buf) == WORD_LEN && dict_size < MAX_WORDS)
            dictionary[dict_size++] = strdup(buf);
    }
    fclose(f);
    return dict_size;
}

bool is_valid_word(const char *w)
{
    for (int i = 0; i < dict_size; i++)
        if (strcmp(dictionary[i], w) == 0)
            return true;
    return false;
}

void free_dictionary()
{
    if (!dictionary)
        return;
    for (int i = 0; i < dict_size; i++)
        free(dictionary[i]);
    free(dictionary);
    dictionary = NULL;
    dict_size = 0;
}

/* ---------------- Feedback ---------------- */
void compute_feedback(const char *guess, const char *target, Color colors[])
{
    int count[26] = {0};
    for (int i = 0; i < WORD_LEN; i++)
        colors[i] = GRAY;
    for (int i = 0; i < WORD_LEN; i++)
    {
        if (guess[i] == target[i])
            colors[i] = GREEN;
        else
            count[target[i] - 'a']++;
    }
    for (int i = 0; i < WORD_LEN; i++)
    {
        if (colors[i] == GREEN)
            continue;
        int idx = guess[i] - 'a';
        if (count[idx] > 0)
        {
            colors[i] = YELLOW;
            count[idx]--;
        }
        else
            colors[i] = GRAY;
    }
}

void print_feedback(const char *guess, Color colors[])
{
    for (int i = 0; i < WORD_LEN; i++)
    {
        printf("[");
        if (colors[i] == GREEN)
            printf("\x1b[42m\x1b[30m%c\x1b[0m", toupper(guess[i]));
        else if (colors[i] == YELLOW)
            printf("\x1b[43m\x1b[30m%c\x1b[0m", toupper(guess[i]));
        else
            printf("\x1b[100m\x1b[37m%c\x1b[0m", toupper(guess[i]));
        printf("] ");
    }
    printf("\n");
}

/* ---------------- Solveur logique anglais corrigé ---------------- */
int solver_user_feedback()
{
    bool used_words[MAX_WORDS] = {0};
    int min_count[26] = {0}; // lettres obligatoires
    int max_count[26];       // lettres max autorisées
    bool correct_pos[WORD_LEN][26] = {0};
    bool wrong_pos[WORD_LEN][26] = {0};

    for (int i = 0; i < 26; i++)
        max_count[i] = WORD_LEN;

    char guesses_history[MAX_GUESSES][WORD_LEN + 1];
    char feedback_history[MAX_GUESSES][WORD_LEN + 1];
    int history_count = 0;

    printf("\n--- SOLVEUR LOGIQUE WORDLE (anglais) ---\n");
    printf("Feedback: g=vert, y=jaune, b=gris\n");

    for (int turn = 1; turn <= MAX_GUESSES; turn++)
    {
        int best_idx = -1;
        int best_score = -1;

        for (int i = 0; i < dict_size; i++)
        {
            if (used_words[i])
                continue;
            char *w = dictionary[i];
            bool valid = true;
            int count_letter[26] = {0};

            for (int j = 0; j < WORD_LEN; j++)
            {
                int c = w[j] - 'a';
                count_letter[c]++;
                if (wrong_pos[j][c])
                {
                    valid = false;
                    break;
                }
                if (correct_pos[j][c] && w[j] - 'a' != c)
                {
                    valid = false;
                    break;
                }
            }
            if (!valid)
                continue;

            // Vérifier min/max count
            for (int c = 0; c < 26; c++)
            {
                if (count_letter[c] < min_count[c] || count_letter[c] > max_count[c])
                {
                    valid = false;
                    break;
                }
            }
            if (!valid)
                continue;

            // Score = lettres encore inconnues
            int score = 0;
            for (int j = 0; j < WORD_LEN; j++)
            {
                int c = w[j] - 'a';
                if (min_count[c] == 0)
                    score++;
            }

            if (score > best_score)
            {
                best_score = score;
                best_idx = i;
            }
        }

        if (best_idx < 0)
        {
            printf("Erreur: aucun mot valide restant !\n");
            break;
        }

        const char *guess = dictionary[best_idx];
        used_words[best_idx] = true;

        printf("\nEssai %d : %s\n", turn, guess);
        if (turn == 1)
            printf("(Premier mot choisi pour maximiser lettres fréquentes)\n");

        printf("Feedback (g/y/b) : ");
        char fb[WORD_LEN + 1];
        if (scanf("%5s", fb) != 1)
        {
            printf("Erreur lecture\n");
            return 1;
        }

        strncpy(guesses_history[history_count], guess, WORD_LEN + 1);
        strncpy(feedback_history[history_count], fb, WORD_LEN + 1);
        history_count++;

        bool win = true;
        int count_guess[26] = {0};
        int g_or_y[26] = {0};

        for (int j = 0; j < WORD_LEN; j++)
        {
            char c = tolower(guess[j]);
            count_guess[c - 'a']++;

            if (fb[j] == 'g')
            {
                correct_pos[j][c - 'a'] = true;
                g_or_y[c - 'a']++;
            }
            else if (fb[j] == 'y')
            {
                wrong_pos[j][c - 'a'] = true;
                g_or_y[c - 'a']++;
            }
            if (fb[j] != 'g')
                win = false;
        }

        // Mettre à jour min/max count pour chaque lettre
        for (int c = 0; c < 26; c++)
        {
            if (g_or_y[c] > 0)
                min_count[c] = (min_count[c] > g_or_y[c] ? min_count[c] : g_or_y[c]);
            if (count_guess[c] > g_or_y[c])
                max_count[c] = g_or_y[c];
        }

        if (win)
        {
            printf("🎉 Mot trouvé : %s en %d essais\n", guess, turn);
            break;
        }
    }

    printf("\n=== HISTORIQUE ===\n");
    for (int i = 0; i < history_count; i++)
        printf("%d) %s -> %s\n", i + 1, guesses_history[i], feedback_history[i]);
    printf("=================\n");

    return 0;
}

/* ---------------- Joueur humain ---------------- */
void human_play()
{
    srand((unsigned)time(NULL));
    int idx = rand() % dict_size;
    char *target = dictionary[idx];
    char guess[64];

    printf("\nDevinez le mot (%d lettres)\n", WORD_LEN);
    for (int t = 1; t <= MAX_GUESSES; t++)
    {
        printf("Essai %d> ", t);
        if (scanf("%63s", guess) != 1)
        {
            printf("Erreur lecture\n");
            return;
        }
        if ((int)strlen(guess) != WORD_LEN)
        {
            printf("Mot de %d lettres !\n", WORD_LEN);
            t--;
            continue;
        }
        for (int i = 0; i < WORD_LEN; i++)
            guess[i] = tolower((unsigned char)guess[i]);
        if (!is_valid_word(guess))
        {
            printf("Mot invalide\n");
            t--;
            continue;
        }
        Color c[WORD_LEN];
        compute_feedback(guess, target, c);
        print_feedback(guess, c);
        if (strcmp(guess, target) == 0)
        {
            printf("Bravo ! Trouvé en %d essais !\n", t);
            return;
        }
    }
    printf("Perdu ! Mot = %s\n", target);
}

/* ---------------- MAIN ---------------- */
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <words.txt>\n", argv[0]);
        return 1;
    }
    int loaded = load_dictionary(argv[1]);
    if (loaded <= 0)
    {
        printf("Dictionnaire vide ou erreur\n");
        return 1;
    }
    printf("%d mots chargés (%d lettres)\n", loaded, WORD_LEN);

    srand((unsigned)time(NULL));

    while (1)
    {
        printf("\n------- Bienvenue dans Wordle Solver anglais -------\n");
        printf("1. Jouer\n2. Solveur logique corrigé\n3. Quitter\n");
        printf("Votre choix : ");

        int ch;
        if (scanf("%d", &ch) != 1)
        {
            while (getchar() != '\n')
                ;
            continue;
        }

        if (ch == 1)
            human_play();
        else if (ch == 2)
            solver_user_feedback();
        else if (ch == 3)
            break;
        else
            printf("Choix invalide\n");
    }

    free_dictionary();
    return 0;
}
