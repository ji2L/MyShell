#include "../include/my_history.h"

void addHistory(char* string, char* path)
{
   FILE* fp;
   
   /*on alloue la mémoire à la chaine qui contiendra le path du fichier history*/
   char* historyPath = malloc((LENGTH + 8) * sizeof(char));
   /*on concatène le nom du fichier au path*/
   sprintf(historyPath, "%s%s", path, "/history");
   /*on alloue la mémoire à la chaine qui contiendra la nouvelle ligne a ajouter à l'historique*/
   char* entry = malloc((LENGTH + 1) * sizeof(char));
   /*on concatène un \n à la fin de la nouvelle entrée pour ne conserver qu'une commande par ligne d'historique*/
   sprintf(entry, "%s%s", string, "\n");

   /*on ouvre le fichier d'historique avec l'option append pour écrire directement à la fin*/
   fp = fopen(historyPath, "a");

   fputs(entry, fp);

   fclose(fp);
   free(historyPath);
   free(entry);
}

void addFile(File* file, char* newString, int lineCounter)
{
   /*on alloue la mémoire au nouvel élément que l'on veut ajouter à la file*/
   Element *new = malloc(sizeof(*new));
   /*si la file n'a pas été crée ou si la mémoire n'a pas été allouée correctement, on quitte la fonction*/
   if(file == NULL || new == NULL) exit(EXIT_FAILURE);

   /*on ajoute la chaine au nouvel élément de la file*/
   strcpy(new->string, newString);
   /*on ajoute le numéro de la ligne au nouvel élément de la file*/
   new->lineNumber = lineCounter;
   /*le nouvel élémentne possède pas encore d'élément suivant*/
   new->next = NULL;

   if(file->first != NULL)
   {
      /*la file n'est pas vide, on ajoute le nouvel élément à la fin de la file*/
      Element *currentElement = file->first;
      /*on parcours la file pour se placer à la fin*/
      while(currentElement->next != NULL) currentElement = currentElement->next;
      /*on ajoute le nouvel élément*/
      currentElement->next = new;
   }
   /*sinon la file est vide et on crée le pemrier élément*/
   else file->first = new;
}

char* removeFile(File* file)
{
   /*on alloue la mémoire à la chaine qui servira de retour*/
   char* stringToPrint = malloc(LENGTH * sizeof(char));

   /*si la file est vide, on ne dépile rien*/
   if(file == NULL) exit(EXIT_FAILURE);
   
   if(file->first != NULL)
   {
      /*si la file n'est pas vide, on copie la chaine présente dans la premier élément puis on le supprime et l'élément suivant devient le premier*/
      Element *removedElement = file->first;
      strcpy(stringToPrint, removedElement->string);
      file->first = removedElement->next;
      free(removedElement);
   }

   return stringToPrint;
}

void printFile(File *file)
{
   /*on déclare l'élément qui servira à parcourir la file, il prend donc le premier élément de la file*/
   Element *element = file->first;

   /* si la file est vide, on quitte la fonction*/
   if(file == NULL) exit(EXIT_FAILURE);

   /*sinon on pracours toute la file en affichant chaque élément*/
   while(element != NULL)
   {
      fprintf(stdout, "%s%d%s%s", "  ", element->lineNumber, " ", element->string);
      element = element->next;
   }
}

void history(int spaces, char** args, char* path)
{
   FILE* fp;
   /*lineCOunter servira a numéroter les lignes lors de l'affichage, i est une variable de boucle, indexLineest le numéro d'une ligne*/
   int lineCounter = 0, i = 0, indexLine;
   /*line servira à lire dans le fichier d'historique*/
   char* line = NULL;
   /*historyPath contiendra le path du fichier d'historique*/
   char* historyPath = malloc((LENGTH + 8) * sizeof(char));
   sprintf(historyPath, "%s%s", path, "/history");
   /*extractLine contiendra une ligne du fichier historique*/
   char* extractLine = malloc(LENGTH * sizeof(char));
   /*len et read serviront à lire dans le fichier d'historique*/
   size_t len = 0;
   size_t read;

   /*file servira a afficher un certain nombre de lignes de l'historique*/
   File *file = malloc(sizeof(*file));
   file->first = NULL;

   fp = fopen(historyPath, "r");

   if(fp)
   {
      if(!strcmp(args[0], "history"))
      {
	 if(spaces == 0)
	 {
	    /*dans ce cas on affiche tout l'historique*/
	    while((read = getline(&line, &len, fp)) != -1)
	    {
	       fprintf(stdout, "%s%d%s%s", "  ", lineCounter, " ", line);
	       lineCounter++;
	    }
	 }
	 else if((indexLine = atoi(args[1])) != 0)
	 {
	    /*dans ce cas on affiche les indexLine dernières lignes de l'historique*/
	    while((read = getline(&line, &len, fp)) != -1)
	    {
	       /*i servira a savoir combien d'élément sont dans notre file, tant que i < indexLine on ajoutela nouvelle ligne à la file*/
	       if(i < indexLine)
	       {
		  addFile(file, line, lineCounter);
		  lineCounter++;
		  i++;
	       }
	       /*quand on a indexLine lignes dans la file, on commence par dépiler le premier élément puis on ajoute la nouvelle ligne afin d'avoir toujours indexLine élément dans la file*/
	       else
	       {
		  removeFile(file);
		  addFile(file, line, lineCounter);
		  lineCounter++;
	       }
	    }

	    printFile(file);
	 }
	 else if(args[1][0] == '!')
	 {
	    /*dans ce cas on exécute la n-ième commande de l'historique (format de l'option : !n)*/

	    /*lineNumber contiendra le numéro de la ligne que l'on veut exécuter sans l'option "!" */
	    char* lineNumber = malloc(strlen(args[1]) * sizeof(char));

	    for(i = 1; i < strlen(args[1]); i++) lineNumber[i - 1] = args[1][i];
	    lineNumber[i + 1] = '\0';
	    /*on convertir cette chaine en un entier*/
	    indexLine = atoi(lineNumber);

	    /*on parcours le fichier d'historique pour trouver la bonne ligne*/
	    i = 0;
	    while((read = getline(&line, &len, fp)) != -1 && i <= indexLine)
	    {
	       if(i == indexLine) strcpy(extractLine, line);
	       i++;
	    }

	    if(read != -1)
	    {
	       /*on supprime le \n présent à la fin de extractLine*/
	       char* position = strchr(extractLine, '\n');
	       if(position != NULL) *position = '\0';
	       /*on compte le nombre d'espaces dans la commande*/
	       int spaces = spaceCounter(extractLine);
	       /*on alloue la mémoire au tableau d'arguments de la commande*/
	       char** extractArgs = NULL;
	       /*on parse les arguments dans le tableau args*/
	       extractArgs = argParser(extractLine, spaces);
	       /*si la commande entrée est "cd" on appelle changeDir*/
	       if(!strcmp(extractArgs[0], "cd")) changeDir(extractArgs);
	       /*sinon exécute la commande*/
	       else executeCommand(extractArgs, spaces, path);
	       /*on free args*/
	       for(i = 0; i < (spaces + 2); i++)
	       {
		  extractArgs[i] = NULL;
		  free(extractArgs[i]);
	       }
	       free(extractArgs);
	    }
	    else fprintf(stdout, "%s%d%s", "ERROR : line ", indexLine, " does not exist\n");

	    free(lineNumber);
	 }
      }
      else if(!strcmp(args[0], "!!"))
      {
	 while((read = getline(&line, &len, fp)) != -1)
	 {
	    strcpy(extractLine, line);
	    i++;
	 }
	 
	 if(extractLine != NULL)
	 {
	    char* position = strchr(extractLine, '\n');
	    if(position != NULL) *position = '\0';
	    int spaces = spaceCounter(extractLine);
	    char** extractArgs = NULL;
	    extractArgs = argParser(extractLine, spaces);
	    /*si la commande entrée est "cd" on appelle changeDir*/
	    if(!strcmp(extractArgs[0], "cd")) changeDir(extractArgs);
	    /*sinon exécute la commande*/
	    else executeCommand(extractArgs, spaces, path);
	    /*on free args*/
	    for(i = 0; i < (spaces + 2); i++)
	    {
	       extractArgs[i] = NULL;
	       free(extractArgs[i]);
	    }
	    free(extractArgs);
	 }
	 else fprintf(stdout, "%s", "ERROR : could not exec last line\n");
      }
   }

   free(historyPath);
   free(extractLine);
   free(file);
   free(line);
   fclose(fp);
}
