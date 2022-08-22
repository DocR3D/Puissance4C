#include "fonc.c"
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h> 


//Fonction permettant de jouer à 2 sur un même terminal
int local(){
	clear();
	//Démarre la création du plateau 
	generationZoneDeJeu();
	//Tant que la partie n'est pas finit
	while(verifierVictoire() == 3){
		//Chargement de l'écran de selection de choix
		selectMenu(1);
		numeroJoueur++;
		//Si le second joueur gagne sort de la partie.
		if(verifierVictoire() != 3) break; 
		selectMenu(2);
		numeroJoueur--;
	}
	//Ferme la fenetre
	endwin();
	printf(" %d ",verifierVictoire());
}

//Fonction permettant de créer un serveur ou de se connecter à un serveur existant.
int reseau(){
	int select = 0;
	char * choix1 = "serveur";
	char * choix2 = "connexion";
	//Création d'un menu à deux élément.
	select = menuPrincipal(choix1,choix2);
	clear();
	//Si le premier choix a été fait
	if(select %2 ==1){
		//Chargement de la vue pour se connecter à un serveur
		connexionServeur();
	//Si c'est le deuxieme choix
	}else {
		//Chargement de la vue permettant de créer un serveur
		creerServeur();
	}
	endwin();
}


int main(){
	printf("%d",htonl(INADDR_LOOPBACK));
	int select = 0;
	char * choix1 = "local";
	char * choix2 = "reseau";
	//Initialise l'écran
	initscr();    	
	
	//Démares les couleurs	
	start_color();

	//Créer un menu à deux élements.
	select = menuPrincipal(choix1,choix2);
	endwin();
	//Jouer à 2 sur le même terminal
	if(select % 2 ==0) local();
	else {
		//Jouer en réseau 
		reseau();
		game();
	}

}


