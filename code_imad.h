#ifndef WORDLE_H          // Débute la garde d'inclusion (include guard).
#define WORDLE_H          // Définit le symbole WORDLE_H pour éviter les inclusions multiples.

#include <stdbool.h>      // Inclut la bibliothèque pour utiliser le type bool (true/false).

#define WORD_LEN 5        // Définit la longueur standard du mot à deviner (5 lettres).
#define MAX_WORDS 20000   // Définit le nombre maximum de mots que le dictionnaire peut contenir.
#define MAX_GUESSES 6     // Définit le nombre maximum de tentatives autorisées par partie.

// Définit un type énuméré pour représenter le retour de couleur après une tentative.
typedef enum {
    GRAY = 0,             // GRAY (Gris) : La lettre n'est pas dans le mot cible.
    YELLOW = 1,           // YELLOW (Jaune) : La lettre est dans le mot cible mais à une mauvaise position.
    GREEN = 2             // GREEN (Vert) : La lettre est dans le mot cible et à la bonne position.
} Color;

// Déclaration des fonctions

// Charge les mots valides à partir d'un fichier dans la mémoire (dictionnaire).
// Prend en paramètre le nom du fichier. Retourne le nombre de mots chargés.
int load_dictionary(const char *filename);

// Vérifie si la chaîne de caractères 'w' est un mot valide (dans le dictionnaire).
// Retourne true si oui, false sinon.
bool is_valid_word(const char *w);

// Libère la mémoire allouée dynamiquement pour le dictionnaire.
void free_dictionary();

// Calcule le retour de couleurs ('colors') pour une 'guess' (tentative) par rapport au 'target' (mot cible).
// Le tableau 'colors' doit être de taille WORD_LEN.
void compute_feedback(const char *guess, const char *target, Color colors[]);

// Affiche le retour de couleurs ('colors') et la 'guess' (tentative) à l'utilisateur.
void print_feedback(const char *guess, Color colors[]);

// Fonction pour le mode solveur (machine qui devine) en interagissant avec l'utilisateur pour le feedback.
// Retourne le nombre de tentatives nécessaires.
int solver_user_feedback();

// Fonction pour le mode de jeu 'humain' (joueur qui devine).
void human_play();

#endif                    // Termine la garde d'inclusion.
