#compilateur
CC = gcc

#option de compilation
CFLAGS = -Wall -ansi -pedantic -std=c99

#fichiers objet à créer
OBJS = src/obj/my_shell.o src/obj/my_parsing.o src/obj/my_commands.o src/obj/my_history.o src/obj/my_copy.o

#nom de l'exécutable
PROG = myshell

#règle all (compile puis lance l'exécutable après s'être placé au bon endroit)
all : $(OBJS)
	 $(CC) -o $(PROG) $(OBJS)

#règle pour le main
src/obj/my_shell.o : src/my_shell.c
	 $(CC) -o $@ -c $< $(CFLAGS)

#règle pour my_parsing.c
src/obj/my_parsing.o : src/my_parsing.c
	 $(CC) -o $@ -c $< $(CFLAGS)

#règle pour my_commands.c
src/obj/my_commands.o : src/my_commands.c
	 $(CC) -o $@ -c $< $(CFLAGS)

#règle pour my_history.c
src/obj/my_history.o : src/my_history.c
	 $(CC) -o $@ -c $< $(CFLAGS)

#règle pour my_copy.c
src/obj/my_copy.o : src/my_copy.c
	 $(CC) -o $@ -c $< $(CFLAGS)

#règle pour nettoyer (make clean) supprime les .o et l'exécutable
clean :
	 -rm src/obj/*.o $(PROG)
