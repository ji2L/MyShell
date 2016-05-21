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
#include "../include/my_commands.h"
#include "../include/my_parsing.h"
#include "../include/my_copy.h"

/*taille max des commandes entrees*/
const int max_length = 1000;
/*taille max des arguments*/
const int arg_length = 1000;

int main(int argc, char *argv[])
{
   /*path contiendra le repertoire courant lors du lancement du shell (il ne sera plus modifié)*/
   char* path = (char*)malloc(max_length * sizeof(char));
   getcwd(path, arg_length);

   /*string contiendra les commandes entrée par l'utilisateur*/
   char string[max_length];

   /*name contiendra le hostname*/
   char* name = (char*)malloc(arg_length * sizeof(char));
   /*networkPosition contiendra la position du '.' indiquant une machine réseau dans name*/
   char* networkPosition = NULL;
   /*currentDir contiendra le repertoire courant*/
   char* currentDir = (char*)malloc(arg_length * sizeof(char));
   /*commandLine contiendra la chaine a affichée avant les commandes*/
   char* commandLine = (char*)malloc(max_length * sizeof(char));

   /*args contiendra les arguments de la commande entrée*/
   char** args = NULL;

   int i, spaces, exiting = 1;

   /*on recupere le hostname*/
   if(gethostname(name, arg_length) == -1)
   {
      fprintf(stdout, "%s%s%s", "gethostname failed : ", strerror(errno), "\n");
      return 0;
   }
   networkPosition = strchr(name, '.');

   /*si name contient un '.' cela indique un nom de la forme machine.reseau, on coupe la partie reseau*/
   if(networkPosition) networkPosition = '\0';

   /*boucle principale*/
   while(exiting)
   {
      getcwd(currentDir, arg_length);
      /*on concatene les différentes parties de l'invite de commande, getlogin() permet de recuperer le login actuel*/
      sprintf(commandLine, "%s%s%s%s%s%s", getlogin(), "@", name, ":", currentDir, ">");

      fprintf(stdout, commandLine);

      /*on recupere la commande entrée par l'utilisateur*/
      exiting = getString(string, max_length);

      /*on compte le nombre d'espaces dans la commande*/
      spaces = spaceCounter(string);

      /*on parse les arguments dans args*/
      args = argParser(string, spaces);

      /*on ajoute la commande entrée à history si elle n'est pas vide et est différent de !!*/
      if(strcmp(args[0], "\0") && strcmp(args[0], "!!")) addHistory(string, path);

      /*on appelle la fonction qui execute les commandes*/
      if(strcmp(args[0], "\0"))
      {
	 if(!strcmp(args[0], "exit")) exiting = 0;
	 else if(!strcmp(args[0], "cd")) changeDir(args);
	 else executeCommand(args, spaces, path);
      }

      for(i = 0; i < (spaces + 2); i++)
      {
	 args[i] = NULL;
	 free(args[i]);
      }
      free(args);
   }

   free(name);
   free(commandLine);
   free(currentDir);
   free(path);

   return 1;
}
