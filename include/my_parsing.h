#ifndef _MY_PARSING
#define _MY_PARSING

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include "../include/my_history.h"
#include "../include/my_commands.h"
#include "../include/my_copy.h"
#include "../include/my_parsing.h"

#define LENGTH 1000

/*Fonction getString
 * Entrees : 1 chaine de characteres, la longueur maximale de la chaine lue
 * Sorties : 1 si la lecture s'est bien passee, 0 sinon
 * 
 * Lit la chaine de characteres entree au clavier.
 */
int getString(char *string, int length);

/*Fonction argParser
 * Entrees : 1 tableau de chaines de characteres, 1 chaine de characteres et le nombres d'espaces dans cette chaine
 * Sorties : auncune
 * 
 * Parse les arguments (i.e: -a) presents dans la chaine de characteres.
 */
char** argParser(char *string, int spaces);

/*Fonction spaceCounter
 * Entrees : 1 chaine de characteres
 * Sorties : 1 int
 * 
 * Compte le nombre d'espaces dans la chaine.
 * Ne compte pas les repetitions d'espaces, ni les espaces precedents la permiere lettre.
 */
int spaceCounter(char *string);

/*Fonction findPath
 * Entrées : le tableau d'argument contenant la commande entrée par l'utilisateur
 * Sorties : 1 si succès 0 sinon
 * Parcours le PATH pour exécuter une commande externe si cela est possible
 */
int findPath(char** args);

/*Fonction executeCommand
 * Entrées : le  tableau d'arguments contenant la commande entrée par l'utilisateur et le nombre despaces qu'elle contient et le path vers le dossier du projet qui contient history
 * Sorties : aucune
 * Exécute la commande entrée par l'utilisateur.
 */
void executeCommand(char** args, int spaces, char* path);

/*Fonction argParser2
 * Entrées : 1 tableau de chaine de caractères qui contient la commande entrée par l'utilisateur, l'index du pipe précédent la commande que l'on veut extraire et le nombre d'espaces dans la commande entrée par l'utilisateur et le nombre de mots de la commande que l'on parse
 * Sorties : aucune
 * Extrait la partie voulue de la commande entrée par l'utilisateur.
 * Exemple : args contient ls | grep foo, indexCommand = 1
 *	     résultat : command contient grep foo
 */
char** argParser2(char** args, int indexCommand, int spaces, int words);

/*Fonction pipesCounter
 * Entrées : le tableau d'argument contenant la commande entrée par l'utilisateur et le nombre d'espaces qu'elle contient
 * Sorties : le nombre de pipes dans la commande
 * Compte le nombre de '|' dans la commande
 */
int pipesCounter(char** args, int spaces);

/*Fonction wordCounter
 * Entrées : un tableau contenant la commande entrée par l'utilisateur et le nombre d'espaces qu'elle contient, l'index d'un '|' indiquant le début de la commande que l'on souhaite traitée
 * Sorties : le nombre de mots de la commande repérée par indexCommand
 * Compte le nombre de mots de la commande repérer par indexCommand.
 */
int wordCounter(char** args, int spaces, int indexCommand);

/*Fonction getRedirectionPosition
 * Entrées : le tableau d'arguments contenant la commands entrée par l'utilisateur et le nombe d'espaces qu'elle contient
 * Sorties : un tableau d'entiers
 * Trouve la position de la redirection dans la commande entrée par l'utilisateur
 */
int getRedirectionPosition(char** args, int spaces);

#endif
