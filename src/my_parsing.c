#include "../include/my_parsing.h"

int getString(char *string, int length)
{
   char *entryPosition = NULL;

   /*on lit le texte saisi*/
   if(fgets(string, length, stdin) != 0)
   {
      /*on cherche le \n ajoute automatiquement par fgets*/
      entryPosition = strchr(string, '\n');
      /*si on en trouve un, on le remplace par \0*/
      if (entryPosition != NULL) *entryPosition = '\0';

      return 1;
   }

   /*si une erreur est survenue, on retourne 0*/
   else return 0;
}

char** argParser (char *string, int spaces)
{
   /*i permet d'avancer dans string, argCounter et charIndex permettent d'ecrire au bon endroit dans args*/
   int i = 0, argCounter = 0, charIndex = 0;

   char** args = malloc((spaces + 2) * sizeof(char*));
   for(i = 0; i < (spaces + 2); i++) args[i] = NULL;

   i = 0;
   args[0] = malloc(LENGTH * sizeof(char));
   /*on avance dans la chaine si elle commence par des espaces*/
   while(string[i] == ' ') i++;

   /*on parcours string tant qu'on a pas parser le bon nombre d'arguments (spaces + 1) ou qu'on a pas atteint la fin de string*/
   while(argCounter != (spaces + 1) && string[i] != '\0')
   {
      if(string[i] != ' ')
      {
	 /*si string[i] est un charactere, on l'ecrit dans args et on incremente charIndex*/
	 args[argCounter][charIndex] = string[i];
	 charIndex++;
	 i++;
      }
      else
      {
	 /*on avance dans string tant qu'il y a des espaces*/
	 while(string[i] == ' ') i++;
	 /*si on a pas atteint la fin de string, on ajoute un \0 pour terminer le dernier argument entre, on incremente argCounter pour ecrire dans l'argument suivant et on reset charIndex*/
	 if(string[i] != '\0')
	 {
	    args[argCounter][charIndex] = '\0';
	    argCounter++;
	    charIndex = 0;
	    if(argCounter <= spaces) args[argCounter] = malloc(LENGTH * sizeof(char));
	 }
      }
   }

   /*on ajoute le \0 au dernier argument entré*/
   args[argCounter][charIndex] = '\0';

   return args;
}

int spaceCounter(char *string){
   /*i permet de parcourir string, counter permet de compter les espaces*/
   int counter = 0, i = 0;

   /*on avance dans la chaine si elle commence par des espaces*/
   while(string[i] == ' ') i++;

   while(string[i] != '\0'){
      /*si string[i] est un espace, on incremente counter*/
      if(string[i] == ' '){
	 /*on avance dans la chaine tant qu'on rencontre des espaces pour ne compter qu'un espace a la fois*/
	 while(string[i] == ' ' && string[i] != '\0') i++;
	 if(string[i] != '\0')
	 {
	    counter++;
	    i++;
	 }
      }
      else i++;
   }
   
   return counter;
}

int findPath(char** args)
{
   /*indexPath servira a parcourir le PATH, indexBuffer servira a ajouter les caractères au buffer, exit servira a sortir de la boucle lorsqu'on aura trouver le bon path*/
   int indexPath = 0, indexBuffer = 0, exit = 1;
   char* path = malloc(LENGTH * sizeof(char));
   char* buffer = malloc(LENGTH * sizeof(char));

   /*on copie le PATH dans la chaine allouée*/
   strcpy(path, getenv("PATH"));

   /*on parcours le PATH jusqu'à la fin ou jusqu'à trouver l'accès*/
   while(path[indexPath] != '\0' && exit)
   {
      /*les différents paths sont séparés par ':' donc on copie dans le buffer tant qu'on a pas atteint un ':'*/
      if(path[indexPath] != ':')
      {
	 buffer[indexBuffer] = path[indexPath];
	 indexBuffer++;
	 indexPath++;
      }
      else
      {
	 /*lorsqu'on atteint un ':' on ajoute un / et un \0 pour compléter le buffer*/
	 buffer[indexBuffer] = '/';
	 buffer[indexBuffer + 1] = '\0';
	 /*on concatène la commande entrée au path*/
	 strcat(buffer, args[0]);
	 /*on test si le path donne l'accès à la commande voulue*/
	 if(access(buffer, X_OK) == 0)
	 {
	    /*dans ce cas, on remplace args[0] par bufer pour avoir le chemin complet de la commande*/
	    strcpy(args[0], buffer);
	    /*on quitte la boucle*/
	    exit = 0;
	 }
	 else
	 {
	    /*sinon on réinitialise indexBuffer pour pouvoir écrire au début de buffer*/
	    indexBuffer = 0;
	    /*on incrémente indexPath pour passer le ':'*/
	    indexPath++;
	 }
      }
   }

   /*si on ne trouve pas la commande, on affiche un message d'erreur*/
   if(path[indexPath] == '\0')
   {
      fprintf(stdout, "%s%s%s", "  myshell : ", args[0], " : command not found\n");
      free(path);
      free(buffer);
      return 0;
   }

   free(path);
   free(buffer);
   return 1;
}

void executeCommand(char** args, int spaces, char* path)
{
   pid_t child;
   int i, j, status, pipes = pipesCounter(args, spaces), words, redirectionPosition, output, savedStdout, spacesModifier = 0;
   int pipesfds[pipes * 2];
   char** command = NULL;
   char* currentDir = malloc(LENGTH * sizeof(char));
   getcwd(currentDir, LENGTH);

   /*on sauvegarde la sortie standard*/
   savedStdout = dup(1);

   /*si une redirection est présente dans la commande, on redirige la sortie standard sur le fichier spécifié*/
   if((redirectionPosition = getRedirectionPosition(args, spaces)) != -1)
   {
      /*on ouvre le fichier et on le crée s'il n'existe pas*/
      output = open(args[redirectionPosition + 1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
      if(output < 0)
      {
	 perror("ERROR : open failed");
	 exit(EXIT_FAILURE);
      }

      /*on redirige la sortie vers le fichier*/
      dup2(output, 1);
      close(output);

      /*on supprime le '>' et le nom du fichier du tableau d'args et on modifie le nombre d'espaces*/
      for(i = redirectionPosition; i < (spaces + 1); i++) 
      {
	 args[i] = NULL;
	 spacesModifier++;
      }
      spaces -= spacesModifier;
   }

   /*si il y a des pipes dans la commande*/
   if(pipes > 0)
   {
      for(i = 0; i < pipes; i++)
      {
	 if(pipe(pipesfds + i*2) < 0)
	 {
	    perror("ERROR : pipe error");
	    exit(EXIT_FAILURE);
	 }
      } 

      for(i = 0; i < (pipes + 1); i++)
      {
	 child = fork();
	 if(child == -1) fprintf(stderr, "ERROR : fork failed\n");
	 else
	 {
	    /*enfant*/
	    if(child == 0)
	    {
	       words = wordCounter(args, spaces, i);
	       command = argParser2(args, i, spaces, words);
	       
	       /*vers stdout, sauf si c'est la denrière commande*/
	       if(i < pipes)
	       {
		  if(dup2(pipesfds[(i * 2) + 1], 1) < 0)
		  {
		     perror("ERROR : dup2 failed");
		     exit(EXIT_FAILURE);
		  }
	       }

	       /*vers stdin, sauf si c'est la première commande*/
	       if(i != 0)
	       {
		  if(dup2(pipesfds[(i - 1) * 2], 0) < 0)
		  {
		     perror("ERROR : dup2 failed");
		     exit(EXIT_FAILURE);
		  }
	       }

	       /*on ferme les pipes*/
	       for(j = 0; j < (2 * pipes); j++) close(pipesfds[j]);

	       /*si la commande entrée est "history" ou !! on appelle history*/
	       if(!strcmp(command[0], "history") || !strcmp(command[0], "!!"))
	       {
		  history(spaces, command, path);
		  exit(EXIT_SUCCESS);
	       }
	       /*si la commande entrée est "touch" on appelle touch*/
	       else if(!strcmp(command[0], "touch"))
	       {
		  touch(command, spaces);
		  exit(EXIT_SUCCESS);
	       }
	       /*si la commande entrée est "cat" on appelle cat*/
	       else if(!strcmp(command[0], "cat"))
	       {
		  cat(command, spaces);
		  exit(EXIT_SUCCESS);
	       }
	       /*si la commande entrée est "cp" on appelle copy*/
	       else if(!strcmp(command[0], "cp"))
	       {
		  copy(command, spaces);
		  exit(EXIT_SUCCESS);
	       }
	       /*si la commande entrée est "find" on appelle commandFind*/
	       else if(!strcmp(args[0], "find"))
	       {
		  commandFind(currentDir, ".");
		  exit(EXIT_SUCCESS);
	       }
	       else
	       {
		  if(findPath(command))
		  {
		     if(execv(command[0], command) < 0)
		     {
			perror("ERROR : execv failed");
			exit(EXIT_FAILURE);
		     }
		     exit(EXIT_SUCCESS);
		  }
		  else exit(EXIT_FAILURE);
	       }

	       for(j = 0; j < (words + 1); j++)
	       {
		  command[j] = NULL;
		  free(command[j]);
	       }
	       free(command);
	    }
	    else if(child < 0)
	    {
	       perror("ERROR : unknown process");
	       exit(EXIT_FAILURE);
	    }
	 }
      }
      
      /*on ferme les pipes*/
      for(j = 0; j < (2 * pipes); j++) close(pipesfds[j]);
      /*on attends que qhaque processus se finisse*/
      for(j = 0; j < (pipes + 1); j++) wait(&status);
   }
   /*pas de pipes*/
   else
   {
      child = fork();

      if(child == -1) fprintf(stderr, "ERROR : fork failed");
      else
      {
	 /*enfant*/
	 if(child == 0)
	 {
	    /*si la commande entrée est "history" ou !! on appelle history*/
	    if(!strcmp(args[0], "history") || !strcmp(args[0], "!!"))
	    {
	       history(spaces, args, path);
	       exit(EXIT_SUCCESS);
	    }
	    /*si la commande entrée est "touch" on appelle touch*/
	    else if(!strcmp(args[0], "touch"))
	    {
	       touch(args, spaces);
	       exit(EXIT_SUCCESS);
	    }
	    /*si la commande entrée est "cat" on appelle cat*/
	    else if(!strcmp(args[0], "cat"))
	    {
	       cat(args, spaces);
	       exit(EXIT_SUCCESS);
	    }
	    /*si la commande entrée est "cp" on appelle copy*/
	    else if(!strcmp(args[0], "cp"))
	    {
	       copy(args, spaces);
	       exit(EXIT_SUCCESS);
	    }
	    /*si la commande entrée est "find" on appelle commandFind*/
	    else if(!strcmp(args[0], "find"))
	    {
	       commandFind(currentDir, ".");
	       exit(EXIT_SUCCESS);
	    }
	    else
	    {
	       if(findPath(args))
	       {
		  if(execv(args[0], args) < 0)
		  {
		     perror("ERROR : exec failed");
		     exit(EXIT_FAILURE);
		  }
		  exit(EXIT_SUCCESS);
	       }
	       else exit(EXIT_FAILURE);
	    }
	 }
	 /*parent*/
	 else waitpid(child, &status, 0);
      }
   }

   free(currentDir);

   /*on rétablit la sortie standard*/
   dup2(savedStdout, 1);
   close(savedStdout);
}

char** argParser2(char** args, int indexCommand, int spaces, int words)
{
   /*i servira à parcourir args, j servira à parcourir command et commandCounter servira a nous positionner correctement dans args*/
   int i = 0, j = 0, commandCounter = 0;

   char** command = malloc((words + 1) * sizeof(char*));
   for(i = 0; i < (words + 1); i++) command[i] = NULL;
   command[0] = malloc(LENGTH * sizeof(char));

   i = 0;

   /*on avance dans le tableau args jusqu'à la commande voulue*/
   while(commandCounter < indexCommand)
   {
      if(!strcmp(args[i], "|")) commandCounter++;
      i++;
   }

   /*on parse la commande dans command*/
   while(j < words && i <= spaces)
   {
      strcpy(command[j], args[i]);
      i++;
      j++;
      if(j < words) command[j] = malloc(LENGTH * sizeof(char));
   }

   return command;
}

int pipesCounter(char** args, int spaces)
{
   int i = 0, pipesCounter = 0;

   /*on parcours args en comptant les '|'*/
   while(i <= spaces)
   {
      if(!strcmp(args[i], "|")) pipesCounter++;
      i++;
   }

   return pipesCounter;
}

int wordCounter(char** args, int spaces, int indexCommand)
{
   int i = 0, counter = 0, wordCounter = 0, exit = 1;

   /*on avance dans le tableau args jusqu'à la commande voulue*/
   while(counter < indexCommand)
   {
      if(!strcmp(args[i], "|")) counter++;
      i++;
   }
   
   /*on compte le nombre de mots présents dans la commande désignée par indexCommand*/
   while(exit && i <= spaces)
   {
      if(strcmp(args[i], "|")) wordCounter++;
      else exit = 0;
      i++;
   }

   return wordCounter;
}

int getRedirectionPosition(char** args, int spaces)
{ 
   for(int i = 0; i < (spaces + 1); i++)
   {
      if((!strcmp(args[i], ">")) && (i < spaces)) return i;
   }
   
   return -1;
}
