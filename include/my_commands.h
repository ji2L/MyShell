#ifndef _MY_COMMANDS
#define _MY_COMMANDS

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <dirent.h>
#include "../include/my_commands.h"
#include "../include/my_parsing.h"

#define LENGTH 1000

/*Fonction changeDir
 * Entrées : 1 tableau a 2 dimensions contennant la commande entrée par l'utilisateur découpée
 * Sorties : aucune
 *
 * Gere tout les comportement habituel de la commande cd
 */
void changeDir(char** args);

/*Fonction touch
 * Entrées : un tableau contenant les arguments de la commande entrée et le nombre d'espaces dans cette commande
 * Sorties : aucune
 * Format : touch [OPTION] [FILE 1] [FILE 2] ...
 * Commande touch, gère l'option "-m" qui ne met à jour que la date de modification
 */
void touch(char** args, int spaces);

/*Fonction cat
 * Entrée : un tableau d'arguments contenant la commande entrée par l'utilisateur et le nombre d'espaces dans cette commande
 * Sorties : aucune
 * Commande cat, gère l'option "-n" et le passage de plusieurs fichiers en arguments
 */
void cat(char** args, int spaces);

/*Fonction commandFind
 * Entrée : una tableau de caractères contenant le path à partir duquel on exécute la commande et le chemin du répertoire parent dans le cas du parcours récursif des répertoires
 * Sorties : aucune
 * Commande find, ne gère aucune option
 */
void commandFind(char* path, char* parentPath);

#endif
