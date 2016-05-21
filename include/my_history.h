#ifndef _MY_HISTORY
#define _MY_HISTORY

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include "../include/my_history.h"
#include "../include/my_parsing.h"
#include "../include/my_commands.h"

#define LENGTH 1000

typedef struct Element Element;
struct Element
{
   char string[LENGTH];
   int lineNumber;
   Element *next;
};

typedef struct File File;
struct File
{
   Element *first;
};

/*Fonction addHistory
 * Entrées : un tableau de char contenant la commande entrée par l'utilisateur, un tableau de char contenant le path du fichier history
 * Sorties : aucune
 * Ajoute la commande entrée par l'utilisateur au fishier sauvegardant l'historique des commandes
 */
void addHistory(char* string, char* path);

/*Fonction history
 * Entrées : la commande entrée par l'utilisateur, le nombre d'espaces présents dans cette commande, le path du fichier contenant l'historique
 * Sorties : aucune
 * Si l'utilisateur entre la commande "history", affiche le contenu de l'historique.
 * Si l'utilisateur entre la commande 'history n" affiche les n dernières lignes de l'historique.
 * Si l'utilisateur entre la commande "history !n" exécute la ligne n de l'historique.
 * Si l'utilisateur entre la commande "!!" exécute la dernière commande de l'historique.
 */
void history(int spaces, char** args, char* path);

/*Fonction add
 * Entrées : une file, la nouvelle chaine a ajouter et le numéro de cette ligne
 * Sorties : aucune
 * Ajoute la chaine a la file.
 */
void addFile(File* file, char* newString, int lineCounter);

/*Fonction remove
 * Entrées : une file
 * Sorties : la chaine de l'élément retiré
 * Retire le premier élément de la file.
 */
char* removeFile(File* file);

/*Fonction printFile
 * Entrées : une file
 * Sorties : aucune
 * Affiche le contenu de la file.
 */
void printFile(File *file);

#endif
