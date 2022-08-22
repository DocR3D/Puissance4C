main: main.o serveur.o fonc.o
	gcc main.c -o main -lncurses
main.o : main.c
	gcc -o main.o -c main.c -lncurses
serveur.o: serveur.c
	gcc -o serveur.o -c serveur.c -lncurses
fonc.o: fonc.c
	gcc -o fonc.o -c fonc.c -lncurses
