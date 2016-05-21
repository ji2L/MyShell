#ifndef _MY_COPY
#define _MY_COPY

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include "../include/my_copy.h"

#define CP_SIZE 1024

/*Fonction copyFile
 * Entrées : deux tableaux de caractères contenant le nom du fichier et le nom de la destination
 * Sories : 1 si succès, 0 sinon
 * Copie le fichier source vers la destination.
 */
int copyFile(char source[], char dest[]);

/*Fonction changePermissions
 * Entrées : deux tableaux de caractères contenant le nom du fichier et le nom de la destination
 * Sories : 1 si succès, 0 sinon
 * Donne au fichier/dossier de destination les mêmes droits que ceux de fichier/dossiers sources.
 */
int changePermissions(char source[], char dest[]);

/*Fonction concatenation
 * Entrées : une chiane de caractères temp pour le résultat et 2 chaines de caractères
 * Sorties : aucune
 * Concatène les chaines afin d'obtenir un path correct.
 */
void concatenation(char **temp,char source[],char file[]);

/*Fonction cp
 * Entrées : 2 chaines de caractères
 * Sorties : aucune
 * Réalise la copie des fichiers/dossiers.
 */
void cp(char source[],char dest[]);

/*Fonction copy
 * Entrées : un tableau contenant les arguments de la commande entrée par l'utilisateur et le nombre d'espaces dans cette commande
 * Sorties : aucune
 * Appelle les différentes fonctions qui réalisent la copie.
 */
void copy(char** args, int spaces);


#endif
