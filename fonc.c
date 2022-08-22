#include "struct.h"
#include <stdio.h>
#include <ncurses.h>
#include "serveur.c"
#include <arpa/inet.h>
#include <string.h>
struct TypePlateau unPlateau;
Jeton unJetonBlanc;
const int HORIZONTALEMENT = 1;
const int VERTICALEMENT = 1;
const int DIAGONALEMENT1 = 1;
const int DIAGONALEMENT2 = 1;
int EPAISSEURY;
int EPAISSEURX;

int row,col;
int numeroJoueur = 1;

//Permet de choisir si le jeu se faire en local ou en réseau
int menuPrincipal(char * choix1, char * choix2){
	//Récupère le nombre de ligne et de colonnes de l'écran
	getmaxyx(stdscr,row,col);

	//Définit la taille de chaque pixel selon l'écran
	EPAISSEURY = row;
	EPAISSEURX = col;

	//Active une couleur
	attron(COLOR_PAIR(2));
	//Affiche le choix numéro 1
	mvprintw(EPAISSEURY/2 , EPAISSEURX/3, choix1);
	//désactive une couleur
	attroff(COLOR_PAIR(2));
	//Affiche le choix numéro 2
	mvprintw(EPAISSEURY/2 , 2*EPAISSEURX/3, choix2);
		
	init_pair(2,COLOR_BLACK,COLOR_YELLOW);

	int selected = 0;
	char c;

	//Tant que la touche n'est pas flèche du haut
	while(c != 'A'){
		move(0,0);	
	
		//clrtoeol();
		curs_set(0);
		//Met à jour le curseur selectionné
		//Annule le premier caractère		
		getch();
		//Annule le second caractère
		getch();
		//Récupère le troisième caractère
		c = getch();
		switch(c){
			//Si la touche est flèche gauche, on décrémente pour selectionner l'élement à gauche
			case 'D':
				selected--;
				break;
			//Si la touche est flèche droite, on incrémente pour selectionner l'élément de droite
			case 'C':
				selected++;
				break;
			//Si c'est une autre touche, on ne fait rien
			default:
				break;
		}
		//Colories la case séléctionné
		attron(COLOR_PAIR(2));
		if(selected % 2 == 0){
			mvprintw(EPAISSEURY/2 , EPAISSEURX/3, choix1);
			attroff(COLOR_PAIR(2));
			mvprintw(EPAISSEURY/2 , 2*EPAISSEURX/3, choix2);
		}else{
			mvprintw(EPAISSEURY/2 , 2*EPAISSEURX/3, choix2);
			attroff(COLOR_PAIR(2));
			mvprintw(EPAISSEURY/2 , EPAISSEURX/3, choix1);
		}
	}
	refresh();             
	//Renvoie la coordonnée X choisis
	return selected;
}


//Fonction permettant à un joueur de se connecter
void connexionServeur(){
	numeroJoueur = 2; 

//	char * inputIp = malloc(sizeof(char)*16);
	//char * info = malloc(sizeof(char)*16);
	int port;
	unsigned int uneIp;
	char ip[] = "127.100.100.100";
	//Récupère le nombre de ligne et de colonnes de l'écran
	getmaxyx(stdscr,row,col);

	//Définit la taille de chaque pixel selon l'écran
	EPAISSEURY = row;
	EPAISSEURX = col;
	
	//Récupère l'ip et le port de connexion
	mvprintw(EPAISSEURY/2 , EPAISSEURX/3, "Entrez l'ip :");
	scanw("%s\0",ip);
	mvprintw(2+EPAISSEURY/2 , EPAISSEURX/3, "Entrez le port :");
	scanw("%d",&port);

	// ---- Inverse l'adresse ip : 1.0.0.127 --> 127.0.0.1 	----	//
	char reversed_ip[INET_ADDRSTRLEN];				//
									//
	in_addr_t addr;							//
									//
	inet_pton(AF_INET, ip, &addr);					//
									//
	addr =								// source : https://stackoverflow.com/questions/16373248/convert-ip-for-reverse-ip-lookup
		    ((addr & 0xff000000) >> 24) |			//
		        ((addr & 0x00ff0000) >>  8) |			//
			    ((addr & 0x0000ff00) <<  8) |		//
			        ((addr & 0x000000ff) << 24);		//
									//
	inet_ntop(AF_INET, &addr, reversed_ip, sizeof(reversed_ip));	//
	// -----						----	//

	//Vérifie que l'ip est bien convertissable au bon format	
	if(inet_pton(AF_INET,reversed_ip,&uneIp ) == 0 ) printf("Conversion char to ip non fonctionnel");
	//Tentes de se connecter à l'ip.
	if(!connection(uneIp,port)) printf("connexion échouté");
	endwin();
	
}

//Fonction permettant de créer un serveur.
void creerServeur(){
	 
	//char * ip = malloc(sizeof(char)*16);
	int port;
	//char * info = malloc(sizeof(char)*16);
	getmaxyx(stdscr,row,col);

	//Définit la taille de chaque pixel selon l'écran
	EPAISSEURY = row;
	EPAISSEURX = col;
	
	//Demandes à l'utilisateur le port sur lequel créer le serveur
	mvprintw(EPAISSEURY/2 , EPAISSEURX/3, "Entrez le port :");
	scanw("%d",&port);

	mvprintw(EPAISSEURY/2 , EPAISSEURX/3, "Serveur crée sur le port : %d ! En attente de connexion",port);
	refresh();

	//Créer le serveur et attend une connexion
	if(creationServeur(port) != 0) mvprintw(EPAISSEURY/2 , EPAISSEURX/3, "Connexion réussie !");
	//Affiche un message d'erreur si le serveur n'a pas été crée
	else  mvprintw(EPAISSEURY/2 , EPAISSEURX/3, "Connexion échoué !");
	endwin();
}


//Fonction pour le déroulement d'une partie en réseau.
int game(){
	//Démarre la création du plateau 
	generationZoneDeJeu();
	//Tant que la partie n'est pas finit
	while(verifierVictoire() == 3){
		clear();
		//Si le terminal est le joueur numéro 2
		if(numeroJoueur == 2){
			char * positionX = malloc(sizeof(char));
			//Affiche le menu de choix de position pour placer le jeton, récupère l'information et la convertie en entier
			sprintf(positionX,"%d",selectMenu(2));
			//Envoie la position à l'autre joueur
			if(envoieInfo(positionX) == -1) return 0;
			//Vérifie si une victoire a eu lieu
			if(verifierVictoire() != 3) break; 
			//Néttoies l'écran.
			clear();
			//Affiche l'écran d'attente (attendre que l'autre joueur joue
			afficherGraphique(1,0);
			//récupère la position de l'autre joueur
			positionX = receptionInfo();
			//place le jeton à l'endroit souhaité
			positionner(1,atoi(positionX));
		//Si le terminal est le joueur 1
		}else{ 
			//Affiche l'écran d'attente
			afficherGraphique(2,0);
			char * positionX = malloc(sizeof(char));
			//Récupère la position du jeton de l'autre joueur
			positionX = receptionInfo();
			//Positionne ce jeton
			positionner(2,atoi(positionX));
			//Vérifie si il y a eu une victoire
			if(verifierVictoire() != 3) break;
			//Néttoie l'écran
			clear();
			//affiche l'écran de séléction et places le jeton une fois choisis et converti la position en entier 
			sprintf(positionX,"%d",selectMenu(1));
			//Envoie la position à l'autre joueur
			if(envoieInfo(positionX) == -1) return 0;
	}

		//Si un des joueur gagne sort de la partie.
	}
	clear();
	mvprintw(EPAISSEURY/2 ,EPAISSEURX/2, "Victoire du joueur %d", verifierVictoire() );
	getch();
	endwin();
}



//Génère la zone de jeu en début de partie
void generationZoneDeJeu(){
	//Récupère le nombre de ligne et de colonnes de l'écran
	getmaxyx(stdscr,row,col);

	//Définit la taille de chaque pixel selon l'écran
	EPAISSEURY = row/20;
	EPAISSEURX = col/20;
	//Définit la taille de la zone de jeu
	unPlateau.tailleX = 15;
	unPlateau.tailleY = 10;

	//Définit le nombre de jeton aligné nécéssaire pour gagner
	unPlateau.nombreAligne = 4;
	unJetonBlanc.couleur = blanc;
	//Pour chaque position de la zone de jeu, lui attribue un jeton Blanc
	for(int positionX = 0 ; positionX < unPlateau.tailleX; positionX++)
		for(int positionY = 0 ; positionY < unPlateau.tailleY; positionY++)
			unPlateau.zoneDeJeu[positionX][positionY] = unJetonBlanc;
}
//Affiche le plateau de jeu en ligne de commande
void afficher(){
	printf("affichage\n");
	for(int positionY = unPlateau.tailleY-1 ; positionY >-1 ; positionY--){
		for(int positionX = 0 ; positionX < unPlateau.tailleX; positionX++){
			if(unPlateau.zoneDeJeu[positionX][positionY].couleur == 3){
				printf("|\t");
			}else printf("|%d\t",unPlateau.zoneDeJeu[positionX][positionY].couleur);
		}
		printf("|\n");
	}
}

//Affiche le plateau de façon graphique.
void afficherGraphique(int player, int selected){
	mvprintw(0,0," %d ",selected);	
	
	//Définit la couleur de la case selon si il y a un joueur, lequel, ou non
	init_pair(3,COLOR_BLACK,COLOR_WHITE);
	init_pair(1,COLOR_BLACK,COLOR_RED);
	init_pair(2,COLOR_BLACK,COLOR_YELLOW);

	//Remplit dans un premier temps tout le tableau de pixel blanc
	//Pour chaque positionY
	for(int positionY = 0 ; positionY < unPlateau.tailleY ; positionY++){
		//Pour chaque positionX
		for(int positionX = 0 ; positionX < unPlateau.tailleX ; positionX++){
			//Pour chaque pixel X que doit prendre un jeton
			for(int epaisseurX = 0; epaisseurX <EPAISSEURX; epaisseurX++){
				//Pour chaque pixel Y que doit prendre un jeton
				for(int epaisseurY = 0; epaisseurY <EPAISSEURY; epaisseurY++){
					//Si le jeton est blanc
					if(unPlateau.zoneDeJeu[positionX][positionY].couleur == 3){

						//Active la couleur blanche
						attron(COLOR_PAIR(unPlateau.zoneDeJeu[positionX][positionY].couleur));
						//Si le pixel est la premiere ligne du jeton
						if((epaisseurX*epaisseurY)%(EPAISSEURX*EPAISSEURY) == 0)
							//Met des tirets dans un but esthetique	
							mvprintw(unPlateau.tailleY*EPAISSEURY - positionY*EPAISSEURY+1-epaisseurY ,positionX*EPAISSEURX+1+epaisseurX, "_");
						//Sinon colories juste la case
						else mvprintw(unPlateau.tailleY*EPAISSEURY - positionY*EPAISSEURY+1-epaisseurY ,positionX*EPAISSEURX+1+epaisseurX, " ");
						//Désactive la couleur Blanche
						attroff(COLOR_PAIR(unPlateau.zoneDeJeu[positionX][positionY].couleur));	
					}
				}
			}
			//Si le numéro de joueur réçu en parametre est égal au numéro de joueur du terminal
			if(numeroJoueur == player){
				//Si le numero de jeton est celui selectionné, désactive la couleur pour l'indiquer
				if(positionX == selected) attron(COLOR_PAIR(player));
				//Indiques sous chaque jeton si celui ci est selectionné ou non
				mvprintw(unPlateau.tailleY*EPAISSEURY +1+EPAISSEURY , positionX*EPAISSEURX+1+EPAISSEURX/3, "ICI");
				//Si le numero de jeton est celui selectionné, désactives la couleur pour l'indiquer
				if(positionX == selected) attroff(COLOR_PAIR(player));
			}
		}
	}
	//Si le numéro de joueur réçu en parametre n'est pas égal au numéro de joueur du terminal
	if(numeroJoueur != player){
		//Affiche un message d'attente
		mvprintw(unPlateau.tailleY*EPAISSEURY+EPAISSEURY,0,"En attente de l'autre Joueur");
	}

	//Ajoutes les jetons
	////Pour chaque positionY
	for(int positionY = 0 ; positionY < unPlateau.tailleY ; positionY++){
		//Pour Chaque positionX
		for(int positionX = 0 ; positionX < unPlateau.tailleX ; positionX++){
			//Pour chaque pixel Y que doit prendre le jeton
			for(int epaisseurX = 0; epaisseurX <EPAISSEURX; epaisseurX++){
				//Pour chaque pixel X que doit prendre le jeton
				for(int epaisseurY = 0; epaisseurY <EPAISSEURY; epaisseurY++){
					//Si le jeton n'est pas blanc
					if(unPlateau.zoneDeJeu[positionX][positionY].couleur != 3){
						//Actives la couleur correspondant au jeton
						attron(COLOR_PAIR(unPlateau.zoneDeJeu[positionX][positionY].couleur));
						//Si le pixel est le premier du jeton
						if((epaisseurX*epaisseurY)%(EPAISSEURX*EPAISSEURY) == 0) 
							//Met des tirets dans un but esthetique
							mvprintw(unPlateau.tailleY*EPAISSEURY - positionY*EPAISSEURY+1-epaisseurY ,positionX*EPAISSEURX+1+epaisseurX, "_");
						//Sinon colories juste des cases
						else   	mvprintw(unPlateau.tailleY*EPAISSEURY - positionY*EPAISSEURY+1-epaisseurY ,positionX*EPAISSEURX+1+epaisseurX, " ");
						//Désactives la couleur
						attroff(COLOR_PAIR(unPlateau.zoneDeJeu[positionX][positionY].couleur));	
			
					}
				}
		
			}
		}
			
	}

	move(0,0);	
	
	//clrtoeol();
	curs_set(0);
	// Rafraîchit la fenêtre courante afin de voir le message apparaître
	refresh();             
}


//Ajoutes dans le tableau le jeton à la position
int positionner(int couleur, int positionX){
	int position = -1;
	//Vérification que la position n'est pas en dehors de la zone de jeu
	if(positionX < unPlateau.tailleX){
		// Calcul la position ou tombera le jeton
		for(int positionY = 0 ; positionY < unPlateau.tailleY; positionY++){
			//Verifie que la position comporte un jeton blanc
			if(unPlateau.zoneDeJeu[positionX][positionY].couleur == 3){
				//Ajoute le jeton à l'emplacement et renvoie la position du jeton
				unPlateau.zoneDeJeu[positionX][positionY].couleur = couleur;
				//Renvoies la positionY
				return positionY;
			}
		}
	}
	//Si la position X est en dehors de la zone de jeu ou que le la position X ne possède plus d'emplacement libre, renvoie -1 -1 
	return position;
}

//Fonction permettant si un joueur a remporté la victoire 
int verifierVictoire(){	
	//Parcours toute les positionX de la zone de jeu
	for(int positionX = 0 ; positionX < unPlateau.tailleX; positionX++){
		//Parcours toute les positionY de la zone de Jeu
		for(int positionY = 0; positionY < unPlateau.tailleY; positionY++){
			//Définit la variable isAligne qui par défaut définit à Vrai si un joueur a gagné de façon horizontalement, verticalement et dans les deux diagonales
			int isAligne[4] = { HORIZONTALEMENT , VERTICALEMENT, DIAGONALEMENT1, DIAGONALEMENT2};
			//Verifie si la position du jeton est bien un jeton de joueur
			if(unPlateau.zoneDeJeu[positionX][positionY].couleur != 3){
				//Pour chaque position, vérifie si les n cases suivantes sont toujours les mêmes
				for(int nombreAligne = 0 ; nombreAligne < unPlateau.nombreAligne; nombreAligne++){
					
					
					//Verification en Diagonale (bas gauche à haut droite), Si la vérification des cases suivantes n'entraine pas une sortie de tableau
					if(positionX < unPlateau.tailleX - unPlateau.nombreAligne && positionY < unPlateau.tailleY - unPlateau.nombreAligne){
						//Si une case suivante est differente alors définit la variable isAligne en position diagonale à 0 signifiant, false
						if(unPlateau.zoneDeJeu[positionX][positionY].couleur != unPlateau.zoneDeJeu[positionX+nombreAligne][positionY+nombreAligne].couleur){
							isAligne[2] = 0;
						}
					//Si la vérification entraine une sortie du tableau, alors le joueur ne peut pas gagner en diagonale
					}else isAligne[2] = 0;
					
					
					
					//Verification en Diagonale (bas droite à haut gauche), si la verification des cases suivantes n'entraine pas une sortie de tableau
					if(0 <= positionX - unPlateau.nombreAligne && positionY < unPlateau.tailleY - unPlateau.nombreAligne){ 
						//Si une case suivante est differente alors définit la variable isAligne en position diagonale à 0 signifiant, false
						if(unPlateau.zoneDeJeu[positionX][positionY].couleur != unPlateau.zoneDeJeu[positionX-nombreAligne][positionY+nombreAligne].couleur){
							isAligne[3] = 0;
						}
					//Si la vérification entraine une sortie du tableau, alors le joueur ne peut pas gagner en diagonale
					}else isAligne[3] = 0;


					//Vérification horizontale
					if(positionX <= unPlateau.tailleX - unPlateau.nombreAligne){
						if(unPlateau.zoneDeJeu[positionX][positionY].couleur != unPlateau.zoneDeJeu[positionX + nombreAligne][positionY].couleur){
							isAligne[0] = 0;
						}
					}else isAligne[0] = 0;
					
					
					//Vérification verticale
					if(positionY <= unPlateau.tailleY - unPlateau.nombreAligne){
						if(unPlateau.zoneDeJeu[positionX][positionY].couleur != unPlateau.zoneDeJeu[positionX][positionY + nombreAligne].couleur){
							isAligne[1] = 0;
						}
					}else isAligne[1] = 0 ;
				}
				
					
				//Si après vérification, une des variables est toujours à 1 alors un joueur a gagné	
				if(isAligne[0] == 1 ||isAligne[1] == 1 || isAligne[2] ==1 || isAligne[3] == 1){
					//Renvoie le numéro de la couleure provocant la victoire
					mvprintw(0,24,"%d",unPlateau.zoneDeJeu[positionX][positionY].couleur);
					return unPlateau.zoneDeJeu[positionX][positionY].couleur;
				}		
			}
		
		}
	}
	
	//Renvoie le numéro de la couleure blanche si aucun joueur n'a gagné 
	mvprintw(0,24,"3");
	return 3;

}


//Récupère les inputs du joueurs et met à jour l'affichage
int selectMenu(int couleur){
	int retour = -1;	
	int c= 0;
	int selected = unPlateau.tailleX/2;

	//Tant que la case n'est pas valide
	while( retour == -1){
		//Tant que la touche n'est pas flèche du haut
		while(c != 'A'){
			//Met à jour le curseur selectionné
			afficherGraphique(couleur,selected);
			//Annule le premier caractère		
			getch();
			//Annule le second caractère
			getch();
			//Récupère le troisième caractère
			c = getch();
			switch(c){
				//Si la touche est flèche gauche, on décrémente pour selectionner l'élement à gauche
				case 'D':
					selected--;
					break;
				//Si la touche est flèche droite, on incrémente pour selectionner l'élément de droite
				case 'C':
					selected++;
					break;
				//Si c'est une autre touche, on ne fait rien
				default:
					break;
			}
			//Si le joueur essaie de selectionner l'élément à gauche du premier, selectionne le dernier element
			if(selected < 0) selected = selected + unPlateau.tailleX;
			//Si le joueur essaie de selectionner l'élément à droite du dernier, selectionne le premier element
			if(selected > unPlateau.tailleX-1) selected = selected - unPlateau.tailleX;
		}
		//Tente de poser le jeton et récupère le Y, afin devoir si le jeton a bien été posé.
		retour = positionner(couleur,selected);
	}
	//Renvoie le numéro du menu choisis
	return selected;
}
