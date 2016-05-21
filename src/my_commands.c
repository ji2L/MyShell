#include "../include/my_commands.h"

void changeDir(char** args){
   char* buffer = (char*)malloc(LENGTH * sizeof(char));

   /*cd ~ et cd sans argument ont le même comportement : ils reviennent au home*/
   if(args[1] == NULL || !strcmp(args[1], "~")){
      sprintf(buffer, "%s%s", "/home/", getlogin());
      if(chdir(buffer) != 0) fprintf(stdout, "%s%s%s", "cd failed : ", strerror(errno), "\n");
   }
   else if(chdir(args[1]) != 0)  fprintf(stdout, "%s%s%s", "cd failed : ", strerror(errno), "\n");

   free(buffer);
}

void touch(char** args, int spaces)
{
   /*on crée une variable modify qui vaudra 1 si l'option -m est présente et 0 sinon et une variable startArg qui contien l'indice du nom du premier fichier(2 si l'option est présente, 1 sinon)*/
   int modify = 0, startArg = 1, i;
   struct stat filestat;
   struct utimbuf ubuf;
   /*fp servira a créer les fichiers qui n'existent pas déjà*/
   FILE* fp;

   /*on vérifie si l'otion est présente et on met à jour les variables le cas échéant*/
   if(spaces > 0)
   {
      if(!strcmp(args[1], "-m"))
      {
	 modify++;
	 startArg++;
      }
   }

   for(i = startArg; i <= spaces; i++)
   {
      /*si le fichier n'existe pas, un appel a fopen avec l'option "a" nous assure que le fichier que l'on veut modifier existe bien*/
      if(access(args[i], F_OK) == -1)
      {
	 fp = fopen(args[i], "a");
	 fclose(fp);
      }

      if(modify)
      {
	 /*dans ce cas, on ne modifie que le modify time*/

	 /*on récupère les informations sur le fichier*/
	 if(stat(args[i], &filestat) < 0)
	 {
	    fprintf(stdout, "%s", "ERROR : stat error\n");
	    exit(EXIT_FAILURE);
	 }

	 /*on conserve le access time*/
	 ubuf.actime = filestat.st_atime;
	 /*on met a jour le modify time avec la date actuelle*/
	 time(&ubuf.modtime);
	 /*on appelle utime pour faire la mise à jour*/
	 utime(args[i], &ubuf);

	 fprintf(stdout, "%s%s%s", "touch : ", args[i], " - modify time has been changed.\n");
      }
      else
      {
	 /*dans ce cas, on met à jour access time et modify time avec la date actuelle*/
	 utime(args[i], NULL);
	 fprintf(stdout, "%s%s%s", "touch : ", args[i], " - access and modify time have been changed.\n");

      }
   }
}

void cat(char** args, int spaces)
{
   /*numbers sera passé à 1 si l'option "-n" est présente, startArg sera passé à 2 si l'option "-n" est présente (startArg = indice du nom du premier fichier que l'on veut afficher), exit servira à quitter la boucle de cat, lineNumber servira à afficher les numéros de ligne et i est une variable de boucle*/
   int numbers = 0, startArg = 1, exit = 1, lineNumber = 0, i;
   /*string servira a récupérer les entrée de l'utilisateur*/
   char string[LENGTH];
   FILE* fp;
   /*line, len et read serviront à afficher le contenu des fichiers*/
   char* line = NULL;
   size_t len = 0;
   ssize_t read;
   struct stat info;

   /*si des arguments sont passés en plus de la commande, on vérifie si l'option -n est présente ou non*/
   if(spaces > 0)
   {
      if(!strcmp(args[1], "-n"))
      {
	 /*si l'option est présente, on passe numbers à 1 et startArg à 2*/
	 numbers++;
	 startArg++;
      }
   }

   /*si la commande entrée est "cat" ou "cat -n", on affiche l'entrée standard avec cat*/
   if(spaces == 0 || (spaces == 1 && numbers))
   {
      while(exit)
      {
	 /*on récupère l'entrée de l'utilisateur*/
	 exit = getString(string, LENGTH);

	 /*puis on l'affiche avec ou sans le numéro de ligne suivant l'option*/
	 if(numbers)
	 {
	    fprintf(stdout, "%s%d%s%s%s", " ", lineNumber, " ", string, "\n");
	    lineNumber++;
         }
	 else fprintf(stdout, "%s%s%s", " ", string, "\n");
      }
   }
   else
   {
      /*si la commande contient un ou plusieurs fichiers, on parcours ces fichiers pour en afficher le contenu*/
      for(i = startArg; i <= spaces; i++)
      {
	 /*on test si le nom ne correspond pas à un répertoire*/
	 stat(args[i], &info);
	 if(S_ISDIR(info.st_mode)) fprintf(stdout, "%s%s%s", "cat : ", args[i], " : is a directory\n");
	 else
	 {
	    /*on ouvre le fichier en lecture*/
	    fp = fopen(args[i], "r");
	    if(fp)
	    {
	       /*on affiche le contenu du fichier en ajoutant le numéro de la ligne si besoin*/
	       while((read = getline(&line, &len, fp)) != -1)
	       {
		  if(numbers)
		  {
		     fprintf(stdout, "%s%d%s%s", "   ", lineNumber, " ", line);
		     lineNumber++;
		  }
		  else fprintf(stdout, "%s", line);
	       }

	       fclose(fp);
	    }
	    /*si le fichier n'existe pas, on affiche un message d'erreur*/
	    else fprintf(stdout, "%s%s%s", "cat : ", args[i], " : no such file\n");
	 }
      }
   }

   free(line);
   /*Cat.*/
}

void commandFind(char* path, char* parentPath)
{
   DIR* dir;
   struct dirent *entry;
   struct stat infoDir;
   char* buffer = NULL;
   char* bufferParentPath = malloc(LENGTH * sizeof(char));

   dir = opendir(path);
   if(!dir)
   {
      perror("ERROR : opendir failed");
      exit(EXIT_FAILURE);
   }

   while((entry = readdir(dir)) != NULL)
   {
      buffer = malloc((strlen(path) + strlen(entry->d_name) + 2) * sizeof(char));
      /*on concatène path et le nom du répertoire dans lequel on est*/
      sprintf(buffer, "%s%s%s", path, "/", entry->d_name);
      /*on récupère les info du chemin que l'on vient de créer*/
      if(stat(buffer, &infoDir) == -1)
      {
	 perror("ERROR : stat failed");
	 exit(EXIT_FAILURE);
      }
      /*on ignore '..'*/
      if(strcmp(entry->d_name, ".."))
      {
	 /*si infoDir.st_mode est un répertoire (S_IFDIR) et qu'il n'est pas '.', on affiche son nom et on appel récursivement commandFind pour afficher ce qu'il contient*/
	 if(((infoDir.st_mode) & S_IFMT) == S_IFDIR && strcmp(entry->d_name, "."))
	 {
	    /*on affiche son nom*/
	    fprintf(stdout, "%s%s%s%s", parentPath, "/", entry->d_name, "\n");
	    /*on concatène le nom du répertoire à parentPath pour afficher correctement son contenu*/
	    sprintf(bufferParentPath, "%s%s%s", parentPath, "/", entry->d_name);
	    /*on appel récursivement commandFind*/
	    commandFind(buffer, bufferParentPath);
	 }
	 /*si infoDir est un fichier, on affiche son nom*/
	 else if(strcmp(entry->d_name, ".")) fprintf(stdout, "%s%s%s%s", parentPath, "/", entry->d_name, "\n");
      }

      free(buffer);
   }

   if(closedir(dir))
   {
      fprintf(stdout, "%s", "ERROR : closedir failed\n");
      exit(EXIT_FAILURE);
   }
   free(bufferParentPath);
}
