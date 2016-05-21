#include "../include/my_copy.h"

int copyFile(char source[], char dest[])
{
   char temp[CP_SIZE];
   /* O_RDONLY permet de seulement lire le fichier source */
   int fileSource = open(source,O_RDONLY); 
   /* On donne le droit d'ecriture et de creation sur le fichier de destination */
   int fileDest = open(dest,O_WRONLY | O_TRUNC |O_CREAT,S_IWUSR);
   /* Permet de verifier que tout se passe bien */
   int bienOuvert;

   if(fileDest == -1 || fileSource == -1) return 0;

   /* Boucle qui va copier tout le contenu du fichier source dans le fichier de destination jusqu'a ce que tout soit copie */
   while(1)
   {
      bienOuvert = read(fileSource,temp,CP_SIZE);

      if(bienOuvert != 0) write(fileDest,temp,bienOuvert);

      else
      {
	 if(errno == EAGAIN || errno == EINTR) continue;
	 break;
      }
   }

   if(!changePermissions(source, dest)) return 0;

   close(fileDest);
   close(fileSource);

   return 1;
}

int changePermissions(char source[],char dest[])
{
   struct stat buffer;
   stat(source, &buffer);

   if(!chmod(dest, buffer.st_mode)) return 1;

   else
   {
      printf("Warning : Probleme de changement des permissions\n");
      return 0;
   }
}

void concatenation(char **temp,char source[],char file[])
{
   int length = 0;
   int length2 = 0;

   while(source[length] != '\0') length++;

   while(file[length2] != '\0') length2++;

   if(source[length] != '/')
   {
      /*On alloue de la memoire pour contenir les deux chaines et '/', ensuite on les concatene ensemble*/
      *temp = calloc((length + length2 + 1),sizeof(char));
      strncat(*temp,source,strlen(source));
      strncat(*temp,"/",1);
   }

   else
   {
      /*Ici il y a deja le caractere '/' donc on allue juste la memoire pour contenir les deux chaines*/
      *temp = calloc((length + length2),sizeof(char));
      strncat(*temp,source,strlen(source));
   }

   strncat(*temp,file,strlen(file));
}


void cp(char source[],char dest[])
{
   char *tempSource = NULL;
   char *tempDestination = NULL;
   struct dirent *file = NULL;
   struct stat *fileType = malloc(sizeof(struct stat));
   DIR *sourceFolder = NULL;
   DIR *destFolder = NULL;

   /* On ouvre les repertoires ou les fichiers */
   sourceFolder = opendir(source);
   destFolder = opendir(dest);

   if(sourceFolder == NULL && destFolder == NULL)
   {	
      printf("Warning : Impossible d'ouvrir les repertoire\n");
      return;
   }

   /* Boucle principale assurant la copie */
   while(1)
   {
      file = readdir(sourceFolder);

      if(file == NULL) break;

      else
      {
	 if(file->d_name[0] != '.')
	 {
	    concatenation(&tempSource,source,file->d_name);
	    concatenation(&tempDestination,dest,file->d_name);

	    if(stat(tempSource,fileType) == 0)
	    {
	       /*Si on doit copier un fichier qui n'est pas un dossier*/
	       if(S_ISREG(fileType->st_mode))
	       {
		  changePermissions(tempSource,tempDestination);
		  copyFile(tempSource,tempDestination);
	       }

	       /*Si on doit copier un dossier*/
	       if(S_ISDIR(fileType->st_mode))
	       {
		  mkdir(tempDestination,S_IRWXU);
		  changePermissions(tempSource,tempDestination);
		  /*On appelle la fonction recursivement afin de copier chaque sous dossier*/
		  cp(tempSource,tempDestination);
	       }
	    }

	    free(tempSource);
	    free(tempDestination);
	 }
      }
   }

   if(closedir(sourceFolder) || closedir(destFolder))
   {
      printf("Probleme de fermeture de dossier \n");
      return;
   }
}

void copy(char** args, int spaces)
{
   if(spaces != 2) printf("Nombre d'arguments insuffisants. Il faut au moins 2 noms de repertoires\n");

   struct stat fileType;
   stat(args[1],&fileType);

   if(S_ISDIR(fileType.st_mode))
   {
      mkdir(args[1],S_IRWXU);
      changePermissions(args[1],args[2]);
      /*On appelle la fonction recursivement afin de copier chaque sous dossier*/
      cp(args[1],args[2]);
   }

   else copyFile(args[1],args[2]);
}
